# Writing Multi-Threaded Programs with Pthread

This project is centered around understanding and implementing multi-threaded programming in C using Pthreads. The main application provided demonstrates solving computational problems in parallel, showing the use of threads to manage multiple tasks.

## Component

- **Multi-Threaded Problem Solver (`task.c`):** Utilizes multiple threads to solve a set of computational challenges. It demonstrates the management of threads in a multi-threaded environment using Pthread.

## Compilation

To compile the `task.c` program, which includes linking with the Pthread and OpenSSL libraries, use the following command:

```bash
gcc -ggdb -O0 -o task task.c -lpthread -lcrypto
```

Note: Ensure you have the OpenSSL library installed. If not, you can install it using a package manager, for example:
```bash
sudo apt-get install libssl-dev
```

Execute the compiled program with the number of worker threads and a series of "challenges" as arguments:
```bash
./task <numthreads> <challenge1> <challenge2> ...
```
