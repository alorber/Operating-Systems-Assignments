// Andrew Lorber
// Operating Systems Program 6 - Synchronization
// FIFO Implementation Header File

#ifndef __FIFO_H
#define __FIFO_H

#include "sem.h"

#define MYFIFO_BUFSIZ 4096

typedef struct fifo {
    // Semaphores for reading & writing
    sem fifo_mutex; // Spinlock for reading / writing
    sem full_sem;  // 0 if full
    sem empty_sem; // 0 if empty
    unsigned long fifo_queue[MYFIFO_BUFSIZ];
    int write_index;
    int read_index;
} fifo;

// Initializes the shared memory FIFO *f
void fifo_init(fifo *f);

// Enqueues the data word d into the FIFO, blocking
//      unless and until the FIFO has room to accept 
//      it.
void fifo_wr(fifo *f,unsigned long d);

// Dequeues the next data word from the FIFO and returns it.
unsigned long fifo_rd(fifo *f);

#endif