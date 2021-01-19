// Andrew Lorber
// Operating Systems Program 6 - Synchronization
// Semaphores Implementation Header File

#ifndef __SEM_H
#define __SEM_H

#include "spinlock.h"
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

#define N_PROC 64

// Semaphore Struct
typedef struct sem {
    unsigned int num_resources;      // Number of resources available
    char lock;                       // Mutex lock
    int sleeping_processes[N_PROC];  // Stores list of sleeping process (by ID)
    int num_sleeping;                // Number processes currently sleeping (first available index)
} sem;


// Initializes semaphore *s with the initial count. 
// Initializes any underlying data structures. 
// sem_init should only be called once in the program (per semaphore). 
void sem_init(sem *s, int count);

// Attempts to perform the "P" operation (atomically decrement the semaphore). 
// If this operation would block, return 0, otherwise return 1.
int sem_try(sem *s);

// Performs the "P" operation, blocking until successful.
void sem_wait(sem *s);

// Performs the "V" operation. 
// If any other tasks were sleeping on this semaphore, 
//      wake them by sending a SIGUSR1 to their process 
//      id (which is not the same as the virtual processor number). 
// If there are multiple sleepers (this would happen if multiple 
//      virtual processors attempt the P operation while the count is <1), 
//      then all must be sent the wakeup signal.
void sem_inc(sem *s);


#endif