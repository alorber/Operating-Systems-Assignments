// Andrew Lorber
// OS Program 1

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <string.h>
#include <ctype.h>

#define BUFFSIZE 4096

/* 
kitty - concatenate and copy files

USAGE: 
    kitty [-o outfile] infile1 [...infile2...]
    kitty [-o outfile]

EXIT STATUS:
    Program returns 0 if no errors were encountered.
    Otherwise, it terminates immediately upon error, giving proper error report, and returns -1.
*/

// Function that performs reading and writing to files
// Parameters are input file name (for stderr message), input file descriptor, output file descriptor, and buffer
void kitty(char* f_name, int fd_in, int fd_out, char buff[BUFFSIZE]){
    int b_read = 0; // Bytes read
    int b_written = 0; // Bytes written
    int b_to_write = 0; // Bytes left to write
    int b_transferred = 0; // Total bytes transferred between files
    // Counters to keep track of # read / write calls made
    int num_reads = 0;
    int num_writes = 0;
    int binary = 0; // If file contains binary, 1, else 0
 
    // Reads file --> buffer
    while((b_read = read(fd_in, buff, BUFFSIZE)) > 0) {
        num_reads++;
        b_to_write = b_read;

        // Checks if binary file
        for(int j = 0; j < b_read && !binary; j++) {
            if(!(isprint(buff[j]) || isspace(buff[j]))) {
                binary = 1;
                break;
            }
        }

        // Writes buffer --> file
        while((b_written = write(fd_out, buff[b_read - b_to_write], b_to_write)) != b_to_write) {
            // Checks for error
            if(b_written < 0) {
                fprintf(stderr, "ERROR: There was a problem writing to your output file."
                                "Error code %i: %s\n", errno, strerror(errno));
                                _exit(-1);
            }
            
            num_writes++;
            b_transferred += b_written;

            // Updates bytes left to write if partial write
            b_to_write -= b_written;
        }
        // Updates variables for last write call
        num_writes++;
        b_transferred += b_written;
    }

    // Checks if error
    if(b_read < 0) {
        fprintf(stderr, "ERROR: There was a problem reading your input file %s."
                        "Error code %i: %s\n", f_name, errno, strerror(errno));
        _exit(-1);
    }

    // Prints info message to stderr
    fprintf(stderr, "\n%i bytes transfered from %s to output.\n"
                    "\tThere were %i read system calls and %i write system calls\n", 
                    b_transferred, f_name, num_reads, num_writes);
    // Prints binary warning
    if(binary == 1) {
        fprintf(stderr, "Warning: Binary file.\n");
    }
}


int main(int argc, char *argv[]) {
    
    char buff[BUFFSIZE];
    int fd_in;
    int fd_out = STDOUT_FILENO; // Sets default output to stdout
    int i = 1;  // Counter
  
    // Checks for outfile
    if (argc > 1 && strcmp(argv[1], "-o") == 0) {
        // Checks if outfile is not given with flag
        if(argc == 2) {
            fprintf(stderr, "ERROR: The -o flag requires an additional argument specifying the output file.\n");
            return -1;
        }

        // Opens outfile if exists or creates new file
        fd_out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);

        // Checks for errors opening outfile
        if(fd_out < 0) {
            fprintf(stderr, "ERROR: There was a problem opening your output file %s."
                            "Error code %i: %s\n", argv[2], errno, strerror(errno));
            return -1;
        }

        // Sets current position in argv
        i = 3;
    }

    // Checks if no more arguments
    if(i == argc) {
        // Reads from stdin & writes to fd_out
        kitty("<standard input>", STDIN_FILENO, fd_out, buff);
    }

    // Loops through rest of arguments
    for (; i < argc; i++) {
        // Checks for input of "-"
        if(strcmp(argv[i], "-") == 0) {
            // Reads from stdin & writes to fd_out
            kitty("<standard input>", STDIN_FILENO, fd_out, buff);
        } else {
            // Opens input file
            fd_in = open(argv[i], O_RDONLY);

            // Checks for errors opening infile
            if(fd_in < 0) {
                if(errno == ENOENT) {
                    fprintf(stderr, "ERROR: File %s is not a valid file.\n"
                            "Error code %i: %s.\n", argv[i], errno, strerror(errno));
                } else {
                    fprintf(stderr, "ERROR: There was a problem opening your input file %s."
                                    "\nError code %i: %s\n", argv[i], errno, strerror(errno));
                }
                return -1;
            }
             // Reads from file & writes to fd_out
            kitty(argv[i], fd_in, fd_out, buff);

            // Closes file
            fd_in = close(fd_in);

            // Checks for errors closing
            if(fd_in < 0) {
                fprintf(stderr, "ERROR: There was a problem closing your input file %s."
                                "\nError code %i: %s\n", argv[i], errno, strerror(errno));
                return -1;
            }
        } 
    }
    
    // Closes output file if not stdout
    if(fd_out != STDOUT_FILENO) {
        fd_out = close(fd_out);
        
        // Checks for errors closing
        if(fd_out < 0) {
            fprintf(stderr, "ERROR: There was a problem closing your output file %s."
                            "\nError code %i: %s\n", argv[2], errno, strerror(errno));
            return -1;
        }
    }

    return 0;
}
