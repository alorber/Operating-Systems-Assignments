// Andrew Lorber
// Operating Systems Program 6 - Synchronization
// FIFO Implementation

#include "fifo.h"

// Initializes the shared memory FIFO *f
void fifo_init(fifo *f) {
    // Initializes semaphores
    sem_init(&(f->fifo_mutex), 1); // Spinlock
    sem_init(&(f->full_sem), MYFIFO_BUFSIZ);
    sem_init(&(f->empty_sem), 0);
    f->write_index = 0;
    f->read_index = 0;
}

// Enqueues the data word d into the FIFO
void fifo_wr(fifo *f,unsigned long d) {
    // Blocks until there is room in FIFO
    sem_wait(&(f->full_sem));
    // Blocks until gets spinlock
    sem_wait(&(f->fifo_mutex));

    // Writes to FIFO (Critical Region)
    f->fifo_queue[f->write_index] = d;
    // Updates index (Critical Region)
    // Modulo for circular queue
    f->write_index = ((f->write_index) + 1) % MYFIFO_BUFSIZ;

    // Unlocks
    sem_inc(&(f->fifo_mutex));
    sem_inc(&(f->empty_sem));
}

// Dequeues the next data word from the FIFO and returns it.
unsigned long fifo_rd(fifo *f) {
    // Blocks until there is element in FIFO
    sem_wait(&(f->empty_sem));
    // Blocks until gets spinlock
    sem_wait(&(f->fifo_mutex));

    // Reads from FIFO (Critical Region)
    unsigned long data = f->fifo_queue[f->read_index];
    // Updates index (Critical Region)
    // Modulo for circular queue
    f->read_index = ((f->read_index) + 1) % MYFIFO_BUFSIZ;

    // Unlocks
    sem_inc(&(f->fifo_mutex));
    sem_inc(&(f->full_sem));

    return data;
}