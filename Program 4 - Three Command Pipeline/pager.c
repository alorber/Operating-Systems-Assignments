// Andrew Lorber
// Operating Systems
// Problem Set 4 Question 3 - A Three Command Pipeline

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>

// Pager: Prints "pages" of words

int main(int argc, char* argv[]) {
    // Opens terminal for reading & writing, and checks for errors
    FILE* terminal = fopen("/dev/tty", "w+");
    if(!terminal) {
        fprintf(stderr, "ERROR: Could not open terminal (/dev/tty).\nError Code %i: %s.\n", errno, strerror(errno));
        return -1;
    }

    // Reads words from stdin
    char* word;
    int wordsPrinted = 0;
    size_t n = 0;
    while(getline(&word, &n, stdin) != -1) {
        fprintf(stdout, "%s", word);
        wordsPrinted++;

        // After 23 lines ask for more
        if(wordsPrinted == 23) {
            fprintf(terminal, "---Press RETURN for more---\n");

            // Check for Q / q
            char c = getc(terminal);
            if(c == 81 || c == 113) {
                return 0;
            } 
            
            wordsPrinted = 0;
        }
    }

    // Checks for possible errors
    if(errno == EINVAL || errno == ENOMEM) {
        fprintf(stderr, "ERROR: Unable to read from file %s.\nError Code %i: %s.\n", argv[1], errno, strerror(errno));
        return -1;
    }

    return 0;
}
