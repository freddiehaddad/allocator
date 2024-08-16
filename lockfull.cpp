#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

// Thread safe memory allocator with support for user-specified memory
// alignments.
class AllocFull {
  public:
    // Allocates n bytes of memory with the starting address aligned to
    // alignment.
    void *Malloc(std::size_t bytes, std::size_t alignment) {
        std::uint8_t *address = new std::uint8_t[bytes + alignment];
        std::size_t   start   = reinterpret_cast<std::size_t>(address);
        std::size_t   offset  = alignment - (start % alignment);
        void         *aligned = reinterpret_cast<void *>(start + offset);

        std::lock_guard<std::mutex> lock(_mutex);
        _table[aligned] = address;

        return aligned;
    }

    // Frees the memory allocated by Malloc.
    void Free(void *ptr) {
        std::lock_guard<std::mutex> lock(_mutex);

        void *value = _table[ptr];
        _table.erase(ptr);

        std::uint8_t *address = reinterpret_cast<std::uint8_t *>(value);
        delete[] address;
    }

    ~AllocFull() {
        std::lock_guard<std::mutex> lock(_mutex);
        for (const auto &[key, value] : _table) {
            std::uint8_t *address = reinterpret_cast<std::uint8_t *>(value);
            delete[] address;
        }
    }

  private:
    mutable std::mutex       _mutex;
    std::map<void *, void *> _table;
};

// Lock full data structure for recording thread job results.
class ResultsFull {
  public:
    // Construct a results to hold results from num_threads.
    ResultsFull(std::size_t num_results) : _index(0) {
        _results.resize(num_results);
    }

    // Record results from a thread.
    void record(const std::string &result) {
        std::lock_guard<std::mutex> lock(_mutex);
        _results[_index++] = result;
    }

    // Print the results to standard output.
    void print() const {
        std::lock_guard<std::mutex> lock(_mutex);
        for (auto &result : _results) {
            std::cout << result << std::endl;
        }
    }

  private:
    // Pointer to next index for record.
    std::size_t _index;
    // Container holding the results.
    std::vector<std::string> _results;
    // Lock for synchronization.
    mutable std::mutex _mutex;
};

// Thread worker function.
void do_work(std::size_t allocations, AllocFull &allocator,
             ResultsFull &results) {
    // capture start time
    auto start = std::chrono::high_resolution_clock::now();

    // 1 megabytes
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
    results.record(ss.str());
}

// program entry
int main() {
    const std::size_t num_threads = 16;
    const std::size_t num_tasks   = 50000;

    AllocFull   allocator;
    ResultsFull results(num_threads);

    // capture start time
    auto start = std::chrono::high_resolution_clock::now();
    {
        // spawn thrads to do work and join when finished
        std::vector<std::jthread> pool;
        for (std::size_t i = 0; i < num_threads; i++) {
            pool.emplace_back(std::jthread(
                do_work, num_tasks, std::ref(allocator), std::ref(results)));
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
