// Andrew Lorber
// OS Program 3 - Shell

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

#define BUFFSIZE 4096

// Tokenizes line
void tokenize(char* line, char** tokenList) {
    char* token;
    // Checks if malloc failed
    if(!(token = malloc(BUFFSIZE))) {
        fprintf(stderr, "ERROR: Unable to allocate space for token buffer.\nError Code %i: %s\n", errno, strerror(errno));
        return;
    }

    int position = 0;
    token = strtok(line, " ");
    while(token) {
        // Checks if tokenList is filled & doubles size
        if(position == BUFFSIZE - 1 &&
            !(tokenList = realloc(tokenList, sizeof(tokenList) / sizeof(tokenList[0]) * 2))) {
            fprintf(stderr, "ERROR: Unable to allocate more space for token list.\nError Code %i: %s\n", errno, strerror(errno));
        }

        // Checks for end of line
        if(token != NULL && strcmp(token, "\n") == 0) {
            tokenList[position] = NULL;
            return;
        }

        // Removes newline from end of token
        if(token[strlen(token)-1] == '\n') {
            token[strlen(token)-1] = 0;
        }

        // Adds token to list
        tokenList[position++] = token;

        // Gets next token
        token = strtok(NULL, " ");
    }
    tokenList[position] = NULL;
}

// Executes IO redirect if present
// Returns index of first IO token & -1 on error
int redirectIO(char** tokenList) {
    int command_end = -1; // Last index of command
    char* file_name;
    int fd;
    
    // Checks for malloc error
    if(!(file_name = malloc(BUFFSIZE))) {
        fprintf(stderr, "ERROR: Unable to allocate memory for redirect file name.\nError Code %i: %s\n", errno, strerror(errno));
        return -1;
    }

    // Searches for IO redirect
    int i;
    for(i = 0; tokenList[i] != NULL; i++) {
        int redirected_stdio = -1; // Which std to redirect
        int flags = -1; // Flags for open system call

        // File --> stdin
        if(tokenList[i][0] == '<') {
            if(command_end == -1) { command_end = i; }
            file_name = tokenList[i] + 1;
            redirected_stdio = 0;
            flags = O_RDONLY;
        }
        // Stdout --> file
        else if(tokenList[i][0] == '>'){
            if(command_end == -1) { command_end = i; }
            redirected_stdio = 1;

            // Append
            if(tokenList[i][1] == '>') {
                file_name = tokenList[i] + 2;
                flags = O_WRONLY|O_CREAT|O_APPEND;
            }
            // Truncate
            else {
                file_name = tokenList[i] + 1;
                flags = O_WRONLY|O_CREAT|O_TRUNC;
            }
        }
        // Stderr --> file
        else if(tokenList[i][0] == '2' && tokenList[i][1] == '>') {
            if(command_end == -1) { command_end = i; }
            redirected_stdio = 2;

            // Append
            if(tokenList[i][2] == '>') {
                file_name = tokenList[i] + 3;
                flags = O_WRONLY|O_CREAT|O_APPEND;
            }
            // Truncate
            else {
                file_name = tokenList[i] + 2;
                flags = O_WRONLY|O_CREAT|O_TRUNC;
            }
        }

        // If IO redirect found, opens file
        if(redirected_stdio != -1) {
            // Checks for error opening file
            if((fd = open(file_name, flags, 0666)) == -1) {
                fprintf(stderr, "ERROR: Unable to open IO redirect file %s\nError Code %i: %s\n", file_name, errno, strerror(errno));
                return -1;
            }
            // Checks for error redirecting IO
            if(dup2(fd, redirected_stdio) == -1) {
                fprintf(stderr, "ERROR: Unable to redirect IO to %s\nError Code %i: %s\n", file_name, errno, strerror(errno));
                return -1;
            }
            // Checks for error closing file
            if(close(fd) == -1) {
                fprintf(stderr, "ERROR: Unable to close file %s\nError Code %i: %s\n", file_name, errno, strerror(errno));
                // Doesn't exit, since IO was still redirected.
            }
        }
    }
    if(command_end == -1) { command_end = i; }
    return command_end;
}

// Parses list of tokens
// Returns exit status
int parse(char** tokenList, int exit_status, int input_fd) {
    // Checks for comment
    if(strncmp(tokenList[0], "#", 1) == 0) { return 0; }

    // Checks for cd
    if(strcmp(tokenList[0], "cd") == 0) {
        // Changes directory to argument
        if(tokenList[1] != NULL) {
            if(chdir(tokenList[1]) != 0) {
                fprintf(stderr, "ERROR: Unable to change directories.\nError Code %i: %s\n", errno, strerror(errno));
                return -1;
            }
        }
        // Changes directory to HOME
        else if(chdir(getenv("HOME")) != 0) {
            fprintf(stderr, "ERROR: Unable to change directories.\nError Code %i: %s\n", errno, strerror(errno));
            return -1;
        }
        return 0;
    }

    // Checks for pwd
    else if(strcmp(tokenList[0], "pwd") == 0) {
        char path[1028];
        if(!getcwd(path, 1028)) {
            fprintf(stderr, "ERROR: Unable to get current working directory.\nError Code %i: %s\n", errno, strerror(errno));
            return -1;
        } else {
            // Reports path
            printf("Current working directory: %s\n", path);
            return 0;
        }
    }

    // Checks for exit
    else if(strcmp(tokenList[0], "exit") == 0) {
        // Exits with specified value
        if(tokenList[1] != NULL){
            exit(atoi(tokenList[1]));
        }
        // Exits with stored exit value
        else {
            exit(exit_status);
        }
    }

    // Forks and executes command
    else {
        struct timeval start, end;
        int t_error = 0; // Tracks if error retrieving times
        
        // Checks if error getting start time
        if(gettimeofday(&start, NULL) == -1) {
            fprintf(stderr, "ERROR: Unable to get starting time\nError Code %i: %s\n", errno, strerror(errno));
            t_error = 1;
        }

        int pid = fork();
        int status;
        struct rusage usage;

        // Checks if in child process
        if(pid == 0) {
            // Closes input file
            if(input_fd != STDIN_FILENO){
                if(close(input_fd) == -1) {
                    fprintf(stderr, "ERROR: Unable to close input file.\nError Code %i: %s\n", errno, strerror(errno));
                    return -1;
                }
            }

            int command_end_index; // Last token index of command

            // Checks for IO redirect and establishes if present
            if((command_end_index = redirectIO(tokenList)) == -1) { exit(1); }

            // Sets first IO token to NULL for exec command
            tokenList[command_end_index] = NULL;

            // Checks for error executing the command
            if(execvp(tokenList[0], tokenList) == -1) {
                fprintf(stderr, "ERROR: Unable to execute command %s.\nError Code %i: %s\n", tokenList[0], errno, strerror(errno));
                exit(127);
            }
            return 0; // To stop compiler warning
        }
        // Checks for error forking
        else if(pid == -1) {
            fprintf(stderr, "ERROR: Unable to fork.\nError Code %i: %s\n", errno, strerror(errno));
            return -1;
        }
        //  Checks if in parent process
        else {
            // Checks if error waiting
            if(wait3(&status, 0, &usage) == -1) {
                fprintf(stderr, "ERROR: The child process (pid %i) has exited due to an error.\nError Code %i: %s\n",
                    pid, errno, strerror(errno));
                return -1;
            }

            // Checks if error getting end time
            if(gettimeofday(&end, NULL) == -1) {
                fprintf(stderr, "ERROR: Unable to get ending time\nError Code %i: %s\n", errno, strerror(errno));
                t_error = 1;
            }

            // Reports exit status
            if(WIFEXITED(status)) { printf("The process completed with exit status %i.\n", WEXITSTATUS(status)); }
            // Reports signal that killed process
            else { printf("The process was killed by signal %i.\n", WTERMSIG(status)); }
            // Reports real ellapsed time
            if(t_error == 0) { printf("Real time ellapsed: %u milliseconds.\n", (end.tv_usec - start.tv_usec) * 1000); }
            // Reports user and system time
            printf("User Time: %u milliseconds\nSystem Time: %u milliseconds\n",
                    usage.ru_utime.tv_usec * 1000, usage.ru_stime.tv_usec * 1000);

            return WEXITSTATUS(status);
        }
    }
}

int main(int argc, char* argv[]) {
    // Buffer variables
    char* line = malloc(BUFFSIZE);
    size_t lineSize = BUFFSIZE;
    char** tokenList = malloc(BUFFSIZE);

    // Checks if malloc failed
    if(!line || !tokenList) {
        fprintf(stderr, "ERROR: Unable to allocate space for buffers.\nError Code %i: %s\n", errno, strerror(errno));
        return -1;
    }

    // Checks for argument (input file)
    FILE* in_stream;
    if(argc > 1 ) {
        // Checks for error opening script file
        if(!(in_stream = fopen(argv[1], "r"))) {
            fprintf(stderr, "ERROR: Unable to open script file.\nError Code: %i: %s\n", errno, strerror(errno));
            return -1;
        }
    } else {
        in_stream = stdin;
    }

    int exit_status = -1;
    while(getline(&line, &lineSize, in_stream) != -1) {
        tokenize(line, tokenList);

        // Skips empty lines
        if(tokenList[0] != NULL) {
            exit_status = parse(tokenList, exit_status, fileno(in_stream));
        } 
    }

    return exit_status;
}
