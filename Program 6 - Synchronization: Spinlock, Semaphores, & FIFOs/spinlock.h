// Andrew Lorber
// Operating Systems Program 6 - Synchronization
// Spinlock library header file

#ifndef __SPINLOCK_H
#define __SPINLOCK_H

#include <sched.h>
#include "tas.h"

// Locking function
void spin_lock(char *lock);

// Unlocking function
void spin_unlock(char *lock);

#endif