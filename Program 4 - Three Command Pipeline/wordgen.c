// Andrew Lorber
// Operating Systems
// Problem Set 4 Question 3 - A Three Command Pipeline

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>

#define MAX_WORD_LENGTH 15

// Wordgen: Generates random words

int main(int argc, char* argv[]) {
    int numWords = -1;   // Number of words to generate (-1 = unlimited)

    // Checks for correct argument
    if(argc > 2) {
        fprintf(stderr, "ERROR: Too many arguments.\nThere is 1 optional argument - Number of words to generate.\n");
        return -1;
    } else if(argc == 2) {
        numWords = atoi(argv[1]);  // Assumes argument is valid integer
        if(numWords == 0) {
            numWords = -1;
        }
    }

    char randomWord[MAX_WORD_LENGTH+1];
    char randomLetter;
    int randomLength;

    // Sets random seed
    srand(time(0));

    while(numWords != 0) {
        // Generates random word length (between 3 & 15 inclusive)
        randomLength = (rand() % 13) + 3;

        // Generates random characters and adds to word
        for(int i = 0; i < randomLength; i++) {
            randomLetter = (rand() % 26) + 'A';
            randomWord[i] = randomLetter;
        }
        // Adds null terminator
        randomWord[randomLength] = '\0';

        // Prints word to stdout
        fprintf(stdout, "%s\n", randomWord);

        numWords--;
    }

    return 0;
}

