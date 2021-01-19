// Andrew Lorber
// Operating Systems Program 6 - Synchronization
// FIFO Tester

#include "spinlock.h"
#include "sem.h"
#include "fifo.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

// Number of writers for acid test
#define NUM_WRITERS 10
// Number of times each writer should write
#define NUM_WRITES 50000

// Tests FIFO with single reader & single writer
int easyTest() {
    // Creates shared memory & checks for errors
    void* shared_memory;
    if((shared_memory = mmap(NULL, sizeof(fifo), PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, 0, 0)) == MAP_FAILED) {
        fprintf(stderr, "ERROR: Unable to map shared memory. Error Code %i: %s.\n", errno, strerror(errno));
        return -1;
    }
    fifo *f = (fifo*)shared_memory;

    // Initializes FIFO
    fifo_init(f);

    pid_t pid;
    int fifo_corruption = 0;
    if((pid = fork()) == -1) {
        fprintf(stderr, "ERROR: Unable to fork process. Error Code %i: %s.\n", errno, strerror(errno));
        return -1;
    } 
    // Checks if in child (Writer)
    else if(pid == 0) {
        // Writes to FIFO
        for(unsigned long i = 0; i < NUM_WRITES; i++) {
            fifo_wr(f, i);
        }
        
        // Exits child "process"
        exit(0);
    }
    // Reader
    else {
        // Reads from FIFO & confirms sequential order
        unsigned long prev_val = fifo_rd(f);
        unsigned long curr_val;
        for(int i = 1; i < NUM_WRITES; i++) {
            curr_val = fifo_rd(f);

            // Checks sequential ordering
            if(curr_val != prev_val + 1) {
                fprintf(stderr, "ERROR: Incorrect Order\n%lu -x-> %lu\n", prev_val, curr_val);
                fifo_corruption = 1;
            }

            prev_val = curr_val;
        }
        
        // No corruption found
        if(fifo_corruption == 0) {
            fprintf(stderr, "No corruption found.\n");
        }
    }
    
    return 0;
}

int acidTest() {
    // Creates shared memory & checks for errors
    void* shared_memory;
    if((shared_memory = mmap(NULL, sizeof(fifo), PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, 0, 0)) == MAP_FAILED) {
        fprintf(stderr, "ERROR: Unable to map shared memory. Error Code %i: %s.\n", errno, strerror(errno));
        return -1;
    }
    fifo *f = (fifo*)shared_memory;

    // Initializes FIFO
    fifo_init(f);

    pid_t pid;
    for(int w = 1; w <= NUM_WRITERS; w++) {
        if((pid = fork()) == -1) {
            fprintf(stderr, "ERROR: Unable to fork process. Error Code %i: %s.\n", errno, strerror(errno));
            return -1;
        } 
        // Checks if in a child (Writer)
        else if(pid == 0) {
            // Starting value of writer: 
            //     will be unique for a large amount of writers 
            //     given the set NUM_WRITES.
            unsigned long curr_value = w * 100000; 
            // Writes to FIFO
            for(unsigned long i = 0; i < NUM_WRITES; i++, curr_value++) {
                fifo_wr(f, curr_value);
            }
            
            // Exits child "process"
            exit(0);
        }
    }

    // In Reader
    // Similar to easyTest, want to keep track of the previous value for each
    //    writer. Can do this with an array of len(num_writers).
    unsigned long prev_values[NUM_WRITERS];

    // Resets array to zeros (not a possible number since writerID starts at 1)
    for(int i = 0; i < NUM_WRITERS; i++) {
        prev_values[i] = 0;
    }

    unsigned long curr_val;  // Value received from reading FIFO
    unsigned long prev_val;  // Previous value from given writer
    unsigned long writer_index; // Index of writer in prev_values
    int fifo_corruption = 0; // Whether corruption was found
    for(unsigned long i = 0; i < NUM_WRITERS * NUM_WRITES; i++) {
        // Reads from FIFO
        curr_val = fifo_rd(f);
        writer_index = (curr_val / 100000) - 1;

        // Checks against previous value from writer
        prev_val = prev_values[writer_index];
        if(prev_val != 0 && curr_val != prev_val + 1) {
            fprintf(stderr, "ERROR: Corruption in FIFO\n%lu --X--> %lu\n", prev_val, curr_val);
            fifo_corruption = 1;
        }
    }

    // No corruption found
    if(fifo_corruption == 0) {
        fprintf(stderr, "No corruption found.\n");
    }

    // Confirm children exited
    while(wait(NULL) != -1) {;}

    return 0;
}

// Empty signal handler for SIGUSR1
void emptyHandler(int signo) {}

int main(int argc, char* argv[]) {
    // Stops program from terminating due to SIGUSR1
    struct sigaction sa;
    sa.sa_handler = emptyHandler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        fprintf(stderr, "ERROR: Could not update SIGUSR1 handler.\nError Code %i: %s.\n", errno, strerror(errno));
    }

    fprintf(stderr, "Easy Test:\n-------\n");
    easyTest();
    fprintf(stderr, "\nAcid Test:\n-------\n");
    acidTest();
}