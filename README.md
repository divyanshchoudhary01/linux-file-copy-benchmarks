# Linux File Copy Benchmarks

A collection of Linux file-copy implementations written in C++ to explore and compare different file I/O mechanisms, multithreading strategies, and their performance characteristics.

## Implementations

* [x] mmap
* [x] Parallel mmap
* [x] read/write
* [ ] pread/pwrite
* [ ] readv/writev
* [ ] preadv/pwritev
* [ ] Parallel preadv/pwritev

## Goals

* Compare throughput and scalability of different Linux file-copy APIs.
* Study memory-mapped I/O, scatter-gather I/O, and offset-based file operations.
* Explore multithreaded file-copy techniques using explicit file partitioning.
* Analyze syscall overhead, implementation complexity, and performance trade-offs.
* Gain hands-on experience with Linux systems programming and performance engineering.

## Technologies

* C++17
* Linux/POSIX APIs
* std::thread
* mmap
* readv/writev
* preadv/pwritev
* File-system and I/O performance benchmarking
