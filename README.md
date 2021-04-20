# cpp_concurrency
This repository contains software developed using C++'s concurrency features.

#dependencies
Note that the Julia Set computation utilizes the Boost library (https://www.boost.org), and requires a version >= 1.72.0.
Download information for this library is found here: https://www.boost.org/users/download/
Installation information can be found in section 5 of the Boost docs: https://www.boost.org/doc/libs/1_61_0/more/getting_started/unix-variants.html#or-build-custom-binaries

#Build/Install
`cmake -H. -Btmp` to compile
`cmake --build tmp` to build & link files.

#Queue
An implementation of a concurrent FIFO queue class.
The queue makes use of condition variables, mutexes, and locks to prevent the queue from spinning while waiting for events.

#Thread Pool
The thread pool utilizes the aforementioned queue as a job queue. 
The thread pool serves to avoid the cost overhead of creating/destroying threads.
To this end, threads are only created on construction of a thread pool, destroyed on destruction.

#Julia Set
The Julia set computation (compute_julia_set) serves utilizes the queue and thread pool functionality to compute a fractal set, namely the Julia set.
It is an implementation of a multithreaded algorithm developed to compute an image of the Julia set.
More about the Julia set can be found at the corresponding Wikipedia page: https://en.wikipedia.org/wiki/Julia_set


#Directory structure
The lib folder continas *.cpp files for the thread pool, queue, and julia set library functionality.
While the include directory contains corresponding *.hpp files.
The app folder contains  *.cpp files used for testing purposes.
