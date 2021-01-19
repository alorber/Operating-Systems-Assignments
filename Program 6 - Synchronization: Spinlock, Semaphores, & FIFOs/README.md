# Program 6 - Synchronization: Spinlock, Semaphores, & FIFOs

The goal of this assignment was to explore synchronization issues on a simulated multi-processor, shared-memory environment.
A provided atomic TAS (Test & Set) primitive was used to implement spinlocks. The spinlocks were then used to construct semaphores, which were in turn 
used to build a FIFO that was shared in memory among multiple tasks (both readers and writers).

---

Each stage of this assignment is described in more detail below:

### Test & Set and Spin Lock

The starting point of this assignment was an atomic test and set instruction: ```int tas(volatile char *lock)```

The TAS function works as follows: A zero value of _*lock_ means unlocked and TAS returns the previous value of _*lock_. Therefore, it returns 0 when the lock has 
been acquired, and 1 when it has not.

This atomic TAS was used to implement a spin lock library, which consists of two functions: ```void spin_lock(char *lock)``` and ```void spin_unlock(char *lock)```

A simple test program was written that creates a shared memory region, spawns a bunch of processes sharing it, and does something non-atomic (like incrementing 
an integer in the shared memory). It is shown that without mutex protection provided by the above spinlock/TAS primitive, incorrect results are observed, and that with it, the program consistently 
works.

### Semaphores

A semaphore library was created using the above spinlocks, which implements the four semaphore operations defined below.

1) **Initialization:** Initializes the semaphore with the provided initial count and initializes any underlying data structures.

2) **Attempt Decrement:** Attempts to perform the "P" operation (atomically decrements the semaphore). If this operation would block, returns 0, 
otherwise returns 1.

3) **Decrement:** Performs the "P" operation (atomically decrements the semaphore), blocking until successful.

4) **Increment:** Performs the "V" operation (atomically increments the semaphore). Any other tasks sleeping on this semaphore are woken up to retry 
the "P" operation.

A simple test program was written to test and verify the semaphore library.

### FIFO

A FIFO library was created that maintains a FIFO of unsigned longs using an _mmap_’d shared memory data structure protected and coordinated exclusively with the 
semaphore module developed above. The FIFO allows the following operations:

1) **Initialization:** Initializes the FIFO and any underlying data structures.

2) **Write:** Enqueues the data word into the FIFO, blocking unless and until the FIFO has room to accept it.

3) **Read:** Dequeues the next data word from the FIFO and returns it.

A testing framework was created for the FIFO implementation with a simple test and acid test. 

- **Simple Test:** First, a FIFO is established in shared memory and two virtual processors are created, one writer and one reader. Then, the writer sends a fixed 
number of sequentially-numbered data into the FIFO and the reader reads these, verifying that all are received.

- **Acid Test:** The simple test is implemented with multiple writers and one reader. The reader verifies that all of the writers’ streams were received 
complete, in (relative) sequence, and with no missing or duplicated items. A six-digit writer ID is attached to the beginning of each data word to allow
the reader to differentiate between writers and keep track of the last sequence number seen from each writer. 




