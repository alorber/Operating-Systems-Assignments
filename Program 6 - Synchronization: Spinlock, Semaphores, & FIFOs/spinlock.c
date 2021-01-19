// Andrew Lorber
// Operating Systems Program 6 - Synchronization
// Spinlock library

#include "spinlock.h"

// Locking function
void spin_lock(char *lock) {
    // "spins" while waiting for lock
    while(tas(lock)) {
        sched_yield(); // According to docs, always succeeds
    }
}

// Unlocking function
void spin_unlock(char *lock) {
    *lock = 0;
}