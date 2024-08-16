# Lockless Programming

This is the first part in a series of programming exercises designed to teach
concepts in lockless programming.

There are two different programs that do the same thing. The first is designed
to work without any synchronization primitives. The second uses a mutex to
provide thread safety.

## Lockless

To build the lockless version:

```text
cl /Zi /EHsc /std:c++20 /Fe:lockless .\lockless.cpp
```

Output from run:

```text
Thread 24208 performed 100000 memory allocations in 13.6418306000 seconds.
Thread 22572 performed 100000 memory allocations in 13.5916036000 seconds.
Thread 36480 performed 100000 memory allocations in 13.2632879000 seconds.
Thread  2068 performed 100000 memory allocations in 13.6087935000 seconds.
Thread 18660 performed 100000 memory allocations in 13.4752367000 seconds.
Thread 15380 performed 100000 memory allocations in 13.6117062000 seconds.
Thread 23044 performed 100000 memory allocations in 13.6035864000 seconds.
Thread 27844 performed 100000 memory allocations in 13.6653528000 seconds.
Thread 20792 performed 100000 memory allocations in 13.6350653000 seconds.
Thread 27660 performed 100000 memory allocations in 13.7268693000 seconds.
Thread 29464 performed 100000 memory allocations in 13.6229281000 seconds.
Thread 34280 performed 100000 memory allocations in 13.7123897000 seconds.
Thread 12600 performed 100000 memory allocations in 13.7150322000 seconds.
Thread 33044 performed 100000 memory allocations in 13.7271442000 seconds.
Thread 35604 performed 100000 memory allocations in 13.7135009000 seconds.
Thread 19008 performed 100000 memory allocations in 13.6955812000 seconds.
1600000 memory allocations performed across 16 threads in 13.7315928000 seconds.
```

## Lock Full

To build the lock full version:

```text
cl /Zi /EHsc /std:c++20 /Fe:lockfull .\lockfull.cpp
```

Output from run:

```text
Thread 35652 performed 100000 memory allocations in 17.1347452000 seconds.
Thread 32288 performed 100000 memory allocations in 17.3575454000 seconds.
Thread 22744 performed 100000 memory allocations in 17.3830234000 seconds.
Thread 30956 performed 100000 memory allocations in 17.4004087000 seconds.
Thread 31508 performed 100000 memory allocations in 17.4091533000 seconds.
Thread 24712 performed 100000 memory allocations in 17.4554640000 seconds.
Thread 30788 performed 100000 memory allocations in 17.5130077000 seconds.
Thread 23980 performed 100000 memory allocations in 17.5608125000 seconds.
Thread 33576 performed 100000 memory allocations in 17.5607732000 seconds.
Thread 25204 performed 100000 memory allocations in 17.5831810000 seconds.
Thread 22596 performed 100000 memory allocations in 17.5876817000 seconds.
Thread  4256 performed 100000 memory allocations in 17.5897784000 seconds.
Thread 20080 performed 100000 memory allocations in 17.5909488000 seconds.
Thread 28016 performed 100000 memory allocations in 17.5909777000 seconds.
Thread 30284 performed 100000 memory allocations in 17.5931702000 seconds.
Thread 25660 performed 100000 memory allocations in 17.5948581000 seconds.
1600000 memory allocations performed across 16 threads in 17.5958181000 seconds.
```
