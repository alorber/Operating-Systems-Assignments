// Andrew Lorber
// Operating Systems Problem Set 5 Question 3
// Smear Program

/*
USAGE:
    smear TARGET REPLACEMENT file1 {file 2....}
Replaces each instance of string TARGET with string REPLACEMENT in all of the given files
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    // Checks number of arguments
    if(argc < 4) {
        fprintf(stderr, "ERROR: Incorrect number of input arguments.\nCorrect usage is './smear TARGET REPLACEMENT file1 {file 2....}'.\n");
        return -1;
    }

    // Checks that length of both strings are same
    char* target = argv[1];
    char* replacement = argv[2];
    if(strlen(target) != strlen(replacement)) {
        fprintf(stderr, "ERROR: Strings are not equal length.\n");
        return -1;
    }

    // Searches each file
    for(int file_ind = 3; file_ind < argc; file_ind++) {
        // Opens file and checks for error
        int curr_fd;
        if((curr_fd = open(argv[file_ind], O_RDWR)) < 0) {
            fprintf(stderr, "ERROR: Could not open file %s.\nError Code %i: %s.\n", argv[file_ind], errno, strerror(errno));
            continue; // Checks rest of files
        }

        // Gets file size and checks for error
        struct stat curr_stats;
        if(fstat(curr_fd, &curr_stats) == -1) {
            fprintf(stderr, "ERROR: Could not get length of file %s.\nError Code %i: %s.\n", argv[file_ind], errno, strerror(errno));
            continue; // Checks rest of files
        }
        off_t curr_size = curr_stats.st_size;

        // Maps file into memory and checks for error
        char* curr_file_start;
        if((curr_file_start = mmap(NULL, curr_size, PROT_READ | PROT_WRITE, MAP_SHARED, curr_fd, 0)) == MAP_FAILED) {
            fprintf(stderr, "ERROR: Unable to map file %s into virtual memory.\nError Code %i: %s.\n", argv[file_ind], errno, strerror(errno));
            continue; // Checks rest of files
        }

        // While there are still more instances of TARGET                                 // Moves pointer to end of string
        for(char* curr_loc = curr_file_start; (curr_loc = strstr(curr_loc, target)) != NULL; curr_loc += strlen(target)){
            // Copy REPLACEMENT to position of TARGET
            memcpy(curr_loc, replacement, strlen(target));
        }

        // Unmaps file
        if(munmap(curr_file_start, curr_size) == -1) {
            fprintf(stderr, "ERROR: Unable to unmap file %s.\nError Code %i: %s.\n", argv[file_ind], errno, strerror(errno));
            continue; // Checks rest of files
        }

        // Closes file
        if(close(curr_fd) == -1) {
            fprintf(stderr, "ERROR: Unable to close file %s.\nError Code %i: %s.\n", argv[file_ind], errno, strerror(errno));
            continue; // Checks rest of files
        }
    }

    return 0;
}
