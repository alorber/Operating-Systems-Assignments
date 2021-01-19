// Andrew Lorber
// Operating Systems Program 6 - Synchronization
// Semaphore Tester

#include "spinlock.h"
#include "sem.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_PROCESSES 4  // Using 4, since number of cores
#define NUM_ITERATIONS 2000000

int testSem() {
    // Creates shared memory & checks for errors
    void* shared_memory;
    if((shared_memory = mmap(NULL, sizeof(sem), PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, 0, 0)) == MAP_FAILED) {
        fprintf(stderr, "ERROR: Unable to map shared memory. Error Code %i: %s.\n", errno, strerror(errno));
        return -1;
    }
    sem *s = (sem*)shared_memory;

    // Sets starting value
    int start_val = 0;
    sem_init(s, start_val);

    // Splits loop into half incrementing, half decrementing
    // Should start & end with same value
    int pid;
    for (int p = 0 ; p < NUM_PROCESSES; p++) {
        if ((pid = fork()) == 0) {
            if (p % 2 == 0) {
                for(int j = 0; j < NUM_ITERATIONS; j++) {
                    sem_wait(s);
                }   
            }
            else {
                for(int j = 0; j < NUM_ITERATIONS; j++) {
                    sem_inc(s);
                }
            }
            // fprintf(stderr, "%i sleeping\n", s->num_sleeping);
            fprintf(stderr, "Child %i exiting.\n", p);
            exit(0);
        }
    }
    
    // Once all child processes complete, checks shared value
    while(wait(NULL) != -1) {;}
    fprintf(stderr, "\nExpected value: %i\nActual value: %i\n", start_val, s->num_resources);

    return 0;
}

// Empty signal handler for SIGUSR1
void emptyHandler(int signo) {};

int main(int argc, char* argv[]) {
    // Stops program from terminating due to SIGUSR1
    struct sigaction sa;
    sa.sa_handler = emptyHandler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        fprintf(stderr, "ERROR: Could not update SIGUSR1 handler.\nError Code %i: %s.\n", errno, strerror(errno));
    }

    if (testSem() == -1)
        fprintf(stderr, "ERROR: Was unable to test semaphore.\n");

    return 0;
}