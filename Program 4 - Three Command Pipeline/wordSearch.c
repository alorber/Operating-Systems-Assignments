// Andrew Lorber
// Operating Systems
// Problem Set 4 Question 3 - A Three Command Pipeline

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>

#define WORD_LIST_SIZE 1000000

// Wordsearch: Checks if word is in word list.

int wordsMatched = 0;

void signalHandler(int signal) {
    fprintf(stderr, "%i word(s) matched.\n", wordsMatched);
    exit(0);
}

int main(int argc, char* argv[]) {
    // Adds signal handling
    signal(SIGPIPE, signalHandler);

    // Checks arguments
    if(argc != 2) {
        fprintf(stderr, "ERROR: Invalid arguments.\nThere is 1 argument - Input file.\n");
        return -1;
    }

    // Opens input file & checks for errors
    FILE* infile = fopen(argv[1], "r");
    if(!infile) {
        fprintf(stderr, "ERROR: Could not open file %s.\nError Code %i: %s.\n", argv[1], errno, strerror(errno));
        return -1;
    }

    // Reads in list of words and saves them
    char* wordList[WORD_LIST_SIZE];
    size_t n = 0;
    int numWords;
    for(numWords = 0; getline(&wordList[numWords], &n, infile) != -1; numWords++) {
        // Converts word to uppercase
        for(int i = 0; i < strlen(wordList[numWords]); i++) {
            wordList[numWords][i] = toupper(wordList[numWords][i]);
        }
    }

    // Checks for possible errors
    if(errno == EINVAL || errno == ENOMEM) {
        fprintf(stderr, "ERROR: Unable to read from file %s.\nError Code %i: %s.\n", argv[1], errno, strerror(errno));
        return -1;
    }

    // Closes file & checks for errors
    if(fclose(infile) != 0) {
        fprintf(stderr, "ERROR: Unable to close file %s.\nError Code %i: %s.\n", argv[1], errno, strerror(errno));
        return -1;
    }

    // Reads words from stdin & checks against word list
    char* word;
    while(getline(&word, &n, stdin) != -1) {
        // Checks if word is in list
        for(int i = 0; i < numWords; i++) {
            if(strcmp(word, wordList[i]) == 0) {
                fprintf(stdout, "%s", word);
                wordsMatched++;
            }
        }
    }

    // Checks for possible errors
    if(errno == EINVAL || errno == ENOMEM) {
        fprintf(stderr, "ERROR: Unable to read from STDIN.\nError Code %i: %s.\n", errno, strerror(errno));
        return -1;
    }

    // Prints number of words matched
    fprintf(stderr, "%i word(s) matched.\n", wordsMatched);

    return 0;
}
