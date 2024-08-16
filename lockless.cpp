#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

//     +- memory address returned by new
//     |
// +---v---+--------+-----------+--------------------------------------------+
// | start | offset | meta data | memory aligned at user specified alignment |
// +-------+--------+---v-------+---v----------------------------------------+
//                      |           |
//                      |           +- calculated alignment
//                      |
//                      +- pointer to the starting address
//

// Thread safe memory allocator with support for user-specified memory
// alignments.
class AllocFree {
  public:
    void *Malloc(std::size_t bytes, std::size_t alignment) {
        std::uint8_t *address = new std::uint8_t[bytes + alignment + _meta];
        std::size_t   malloc_start = reinterpret_cast<std::size_t>(address);
        std::size_t   meta_start   = malloc_start + _meta;
        std::size_t   offset       = alignment - (meta_start % alignment);
        std::size_t   user_start   = meta_start + offset;

        meta_start                = user_start - _meta;
        std::size_t *p_meta_start = reinterpret_cast<std::size_t *>(meta_start);
        *p_meta_start             = malloc_start;

        void *aligned = reinterpret_cast<void *>(user_start);
        return aligned;
    }

    void Free(void *ptr) {
        std::size_t  user_start   = reinterpret_cast<std::size_t>(ptr);
        std::size_t  meta_start   = user_start - _meta;
        std::size_t *p_meta_start = reinterpret_cast<std::size_t *>(meta_start);
        std::size_t  malloc_start = *p_meta_start;
        std::uint8_t *address = reinterpret_cast<std::uint8_t *>(malloc_start);
        delete[] address;
    }

  private:
    static const std::size_t _meta = sizeof(std::size_t);
};

// Lockless data structure for recording thread job results.
class Results {
  public:
    // Construct a Results to hold results from num_threads.
    Results(std::size_t num_results) {
        _results.resize(num_results);
    }

    // Record results from a thread at the specified index.
    void record(std::size_t index, const std::string &result) {
        _results[index] = result;
    }

    // Print the results to standard output.
    void print() const {
        for (auto &result : _results) {
            std::cout << result << std::endl;
        }
    }

  private:
    std::vector<std::string> _results;
};

// Thread worker function.
void do_work(std::size_t allocations, std::size_t thread, AllocFree &allocator,
             Results &results) {
    // capture start time
    auto start = std::chrono::high_resolution_clock::now();

    // 1 megabyte
    const std::size_t bytes     = 1 << 20;
    const std::size_t alignment = 128;

    // do some work
    for (std::size_t i = 0; i < allocations; i++) {
        void *ptr = allocator.Malloc(bytes, alignment);
        allocator.Free(ptr);
    }

    // capture end time
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    // generate a result
    std::stringstream ss;
    ss << "Thread " << std::setw(5) << std::right << std::this_thread::get_id()
       << " performed " << allocations << " memory allocations in "
       << std::fixed << std::setprecision(10) << std::right << duration.count()
       << " seconds.";
    // record result at given index.
    results.record(thread, ss.str());
}

// program entry point
int main() {
    const std::size_t num_threads = 16;
    const std::size_t num_tasks   = 50000;

    AllocFree allocator;
    Results   results(num_threads);

    // capture start time
    auto start = std::chrono::high_resolution_clock::now();
    {
        // spawn threads to do work and join when finished
        std::vector<std::jthread> pool;
        for (std::size_t i = 0; i < num_threads; i++) {
            pool.emplace_back(std::jthread(
                do_work, num_tasks, i, std::ref(allocator), std::ref(results)));
        }
    }
    // capture end time
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    results.print();

    std::cout << num_threads * num_tasks
              << " memory allocations performed across " << num_threads
              << " threads in " << std::fixed << std::setprecision(10)
              << duration.count() << " seconds." << std::endl;

    return 0;
}
