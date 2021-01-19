// Andrew Lorber
// Operating Systems
// Problem Set 4 Question 3 - A Three Command Pipeline

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdbool.h>

#define WORD_LIST "words.txt"

// Launcher

int main(int argc, char* argv[]) {
    // Checks arguments
    char* wordgenArgument = "0";
    if(argc > 2) {
        fprintf(stderr, "ERROR: Too many arguments.\nThere is 1 optional argument - Number of words to generate.\n");
    } else if(argc == 2) {
        wordgenArgument = argv[1];
    }

    // Creates pipes & checks for errors
    int fds1[2];
    int fds2[2];
    if(pipe(fds1) != 0 || pipe(fds2) != 0) {
        fprintf(stderr, "ERROR: Unable to create pipes.\nError Code %i: %s.\n", errno, strerror(errno));
    }

    pid_t wordgen_pid, wordsearch_pid, pager_pid;

    // Forks to execute wordgen & checks for errors
    if((wordgen_pid = fork()) == -1) {
        fprintf(stderr, "ERROR: Unable to fork for wordgen.\nError Code %i: %s.\n", errno, strerror(errno));
        return -1;
    }
    // Checks if child process
    else if(wordgen_pid == 0) {
        // Redirects stdout to pipe & checks for errors
        if(dup2(fds1[1], STDOUT_FILENO) == -1) {
            fprintf(stderr, "ERROR: Unable to redirect stdout to pipe for wordgen.\n"
                            "Error Code %i: %s.\n", errno, strerror(errno));
            return -1;
        }

        // Closes pipes & checks for errors
        if(close(fds1[0]) + close(fds1[1]) + close(fds2[0]) + close(fds2[1]) != 0) {
            fprintf(stderr, "ERROR: Unable to close pipes for wordgen.\nError Code %i: %s.\n", errno, strerror(errno));
            return -1;
        }

        // Executes wordgen & checks for errors
        if(execlp("./wordgen", "wordgen", wordgenArgument) == -1) {
            fprintf(stderr, "ERROR: Unable to execute wordgen.\n Error Code %i: %s.\n", errno, strerror(errno));
            return -1;
        }
    }

    // Forks to execute wordsearch & checks for errors
    if((wordsearch_pid = fork()) == -1) {
        fprintf(stderr, "ERROR: Unable to fork for wordsearch.\nError Code %i: %s.\n", errno, strerror(errno));
        return -1;
    }
    // Checks if child process
    else if(wordsearch_pid == 0) {
        // Redirects stdin to pipe & checks for errors
        if(dup2(fds1[0], STDIN_FILENO) == -1) {
            fprintf(stderr, "ERROR: Unable to redirect stdin to pipe for wordsearch.\n"
                            "Error Code %i: %s.\n", errno, strerror(errno));
            return -1;
        }

        // Redirects stdout to pipe & checks for errors
        if(dup2(fds2[1], STDOUT_FILENO) == -1) {
            fprintf(stderr, "ERROR: Unable to redirect stdout to pipe for wordsearch.\n"
                            "Error Code %i: %s.\n", errno, strerror(errno));
            return -1;
        }

        // Closes pipes & checks for errors
        if(close(fds1[0]) + close(fds1[1]) + close(fds2[0]) + close(fds2[1]) != 0) {
            fprintf(stderr, "ERROR: Unable to close pipes for wordsearch.\nError Code %i: %s.\n", errno, strerror(errno));
            return -1;
        }

        // Executes wordsearch & checks for errors
        if(execlp("./wordsearch", "wordsearch", WORD_LIST) == -1) {
            fprintf(stderr, "ERROR: Unable to execute wordsearch.\nError Code %i: %s.\n", errno, strerror(errno));
            return -1;
        }
    }

    // Forks to execute pager & checks for errors
    if((pager_pid = fork()) == -1) {
        fprintf(stderr, "ERROR: Unable to fork for pager.\nError Code %i: %s.\n", errno, strerror(errno));
        return -1;
    }
    // Checks if child process
    else if(pager_pid == 0) {
        // Redirects stdin to pipe & checks for errors
        if(dup2(fds2[0], STDIN_FILENO) == -1) {
            fprintf(stderr, "ERROR: Unable to redirect stdin to pipe for pager.\n"
                            "Error Code %i: %s.\n", errno, strerror(errno));
            return -1;
        }

        // Closes pipes & checks for errors
        if(close(fds1[0]) + close(fds1[1]) + close(fds2[0]) + close(fds2[1]) != 0) {
            fprintf(stderr, "ERROR: Unable to close pipes for pager.\nError Code %i: %s.\n", errno, strerror(errno));
            return -1;
        }

        // Executes pager & checks for errors
        if(execlp("./pager", "pager") == -1) {
            fprintf(stderr, "ERROR: Unable to execute pager.\nError Code %i: %s.\n", errno, strerror(errno));
            return -1;
        }
    }

    // Closes pipes & checks for errors
    if(close(fds1[0]) + close(fds1[1]) + close(fds2[0]) + close(fds2[1]) != 0) {
        fprintf(stderr, "ERROR: Unable to close pipes in launcher.\nError Code %i: %s.\n", errno, strerror(errno));
        return -1;
    }

    // Waits for chlidren
    int status;
    pid_t child;
    while((child = waitpid(-1, &status, 0)) != -1) {
        // Checks for wordgen
        if(child == wordgen_pid) {
            // Checks if signal
            if(WIFSIGNALED(status)) {
                fprintf(stderr, "Wordgen was terminated by signal %d.\n", WTERMSIG(status));
            } else {
                fprintf(stderr, "Wordgen exited with exit status %d.\n", WEXITSTATUS(status));
            }
        } 
        // Checks for wordsearch
        else if(child == wordsearch_pid) {
            // Checks if signal
            if(WIFSIGNALED(status)) {
                fprintf(stderr, "Wordsearch was terminated by signal %d.\n", WTERMSIG(status));
            } else {
                fprintf(stderr, "Wordsearch exited with exit status %d.\n", WEXITSTATUS(status));
            }
        } 
        // Checks for pager
        else if(child == pager_pid) {
            // Checks if signal
            if(WIFSIGNALED(status)) {
                fprintf(stderr, "Pager was terminated by signal %d.\n", WTERMSIG(status));
            } else {
                fprintf(stderr, "Pager exited with exit status %d.\n", WEXITSTATUS(status));
            }
        } else {
            fprintf(stderr, "Someone lost their child, because this one isn't mine...\n");
        }
    }

    return 0;
}
