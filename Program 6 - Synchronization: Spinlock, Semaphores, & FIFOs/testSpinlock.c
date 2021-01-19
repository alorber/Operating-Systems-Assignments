// Andrew Lorber
// Operating Systems Program 6 - Synchronization
// Spinlock Tester

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

#include "spinlock.h"

#define SHARED_SIZE 5000 // Size of shared memory
#define NUM_PROCESSES 4  // Using 4, since number of cores
#define NUM_ITERATIONS 2000000

int noMutex() {
    // Creates shared memory & checks for errors
    void* shared_memory;
    if((shared_memory = mmap(NULL, SHARED_SIZE, PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, 0, 0)) == MAP_FAILED) {
        fprintf(stderr, "ERROR: Unable to map shared memory. Error Code %i: %s.\n", errno, strerror(errno));
        return -1;
    }
    long *shared_value = (long*)shared_memory;

    // Forks NUM_PROCESSES times to create "parallel processes"
    for(int i = 0; i < NUM_PROCESSES; i++) {
        // Forks & checks for errors
        int pid;
        if((pid = fork()) == -1) {
            fprintf(stderr, "ERROR: Unable to fork process %i. Error Code %i: %s.\n", i, errno, strerror(errno));
            return -1;
        } 
        // Checks if in child
        else if(pid == 0) {
            // Increments shared value
            for(int iter = 0; iter < NUM_ITERATIONS; iter++) {
                (*shared_value)++;
            }

            // Exits child "process"
            exit(0);
        }
    }

    // Once all child processes complete, checks shared value
    while(wait(NULL) != -1) {;}
    printf("Expected Value: %i\nActual Value: %li\n", NUM_PROCESSES*NUM_ITERATIONS, *shared_value);
    return 0;
}

// Creates struct for spinlock
struct sharedData {
    char spinlock;
    long value;
} sharedData;

int useMutex() {
    // Creates shared memory & checks for errors
    void* shared_memory;
    if((shared_memory = mmap(NULL, SHARED_SIZE, PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, 0, 0)) == MAP_FAILED) {
        fprintf(stderr, "ERROR: Unable to map shared memory. Error Code %i: %s.\n", errno, strerror(errno));
        return -1;
    }
    struct sharedData *shared_value = (struct sharedData*)shared_memory;
    shared_value->spinlock = 0; // Unlocked at start
    shared_value->value = 0;

    // Forks NUM_PROCESSES times to create "parallel processes"
    for(int i = 0; i < NUM_PROCESSES; i++) {
        // Forks & checks for errors
        int pid;
        if((pid = fork()) == -1) {
            fprintf(stderr, "ERROR: Unable to fork process %i. Error Code %i: %s.\n", i, errno, strerror(errno));
            return -1;
        } 
        // Checks if in child
        else if(pid == 0) {
            // Increments shared value
            for(int iter = 0; iter < NUM_ITERATIONS; iter++) {
                // Locks
                spin_lock(&(shared_value->spinlock));
                // Updates value (Critical Region)
                (shared_value->value)++;
                // Unlocks
                spin_unlock(&(shared_value->spinlock));
            }

            // Exits child "process"
            exit(0);
        }
    }

    // Once all child processes complete, checks shared value
    while(wait(NULL) != -1) {;}
    printf("Expected Value: %i\nActual Value: %li\n", NUM_PROCESSES*NUM_ITERATIONS, shared_value->value);
    return 0;
}

int main(int argc, char* argv[]) {
    printf("Testing spinlock with %i processes & %i iterations.\n", NUM_PROCESSES, NUM_ITERATIONS);

    printf("\nNo Mutex\n------------\n");
    noMutex();

    printf("\nWith Mutex\n------------\n");
    useMutex();

    return 0;
}