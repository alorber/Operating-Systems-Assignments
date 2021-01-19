// Andrew Lorber
// Operating Systems Program 6 - Synchronization
// Semaphores Implementation

#include "sem.h"

// Initializes semaphore
void sem_init(sem *s, int count) {
    // Sets up spinlock
    s->lock = 0;

    // Locks
    spin_lock(&(s->lock));
    // Sets values (Critical Region)
    s->num_resources = count;
    s->num_sleeping = 0;
    // Unlocks
    spin_unlock(&(s->lock));
}

// Attempts to perform the "P" operation
int sem_try(sem *s) {
    // Locks
    spin_lock(&(s->lock));
    // Checks for available resource (Critical Region)
    if(s->num_resources > 0) {
        // Decrements (Critical Region)
        s->num_resources--;
        // Unlocks
        spin_unlock(&(s->lock));
        return 1;
    }
    // No available resources
    else {
        spin_unlock(&(s->lock));
        return 0;
    }
}

// Performs the "P" operation, blocking until successful.
void sem_wait(sem *s) {
    // Creates signal masks
    sigset_t mask, old_mask;
    // Clears new mask
    sigemptyset(&mask);
    // Adds SIGUSR1 to prevent "lost wakeup" problem
    sigaddset(&mask, SIGUSR1);
    // Updates mask
    sigprocmask(SIG_BLOCK, &mask, &old_mask);

    // Locks
    spin_lock(&(s->lock));

    while(s->num_resources == 0) {
        // Adds to waitlist (Critical Region)
        pid_t pid = getpid();
        s->sleeping_processes[s->num_sleeping] = pid;
        s->num_sleeping++;

        // Unlocks
        spin_unlock(&(s->lock));

        // Updates mask - only SIGUSR1 allowed
        sigfillset(&mask);
        sigdelset(&mask, SIGUSR1);

        // Suspends process
        sigsuspend(&mask);

        // Locks
        spin_lock(&(s->lock));
    }

    // Resets signal mask
    sigprocmask(SIG_SETMASK, &old_mask, NULL);

    // Decrements semaphore (Critical Region)
    s->num_resources--;

    // Unlocks
    spin_unlock(&(s->lock));
}

// Performs the V operation
void sem_inc(sem *s) {
    // Locks
    spin_lock(&(s->lock));

    // Increments semaphore (Critical Region)
    s->num_resources++;

    // If there are sleeping processes, wake them up
    if(s->num_resources == 1) {
        for(int i = 0; i < s->num_sleeping; i++) {
            // Sends SIGUSR1 signal (Critical Region)
            if(kill(s->sleeping_processes[i], SIGUSR1) == -1) {
                fprintf(stderr, "ERROR: Could not send signal.\n");
            }
        }
        s->num_sleeping = 0;
    }
    

    // Unlocks
    spin_unlock(&(s->lock));
}