// Andrew Lorber
// OS Program 2

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define BUFFSIZE 4096

// Checks if character is a problematic keyboard character
int isProblematic(char c){
    //     A-Z  a-z 0-9       %          +               - . /              :          =          @          _
    return !(isalnum(c) || c == 37 || c == 43 || (c >= 45 && c <= 47) || c == 58 || c == 61 || c == 64 || c == 95);
}

// Recursively searches directory and collects stats
// @Params: current directory being searched, current path, inode counts, sum of file sizes, sum of allocated disk blocks,
//          number of inodes with 2+ links, number of dangling symbolic links, number of problematic directory names
void getStats(DIR* dir, char* curr_path, unsigned int* inode_counts, unsigned int* f_size_sum, 
                unsigned int* alloc_blk_sum, unsigned int* num_two_plus_links, 
                unsigned int* num_inval_links, unsigned int* num_prob_dir) {
    // Variables
    struct dirent* d_entry;
    struct stat entry_stats;
    char new_path[BUFFSIZE];

    // Gets directory entries
    while((d_entry = readdir(dir))) {
        // Checks for ".." and "."
        if(strcmp(d_entry->d_name, ".") == 0 || strcmp(d_entry->d_name, "..") == 0 || strcmp(d_entry->d_name, "proc") == 0) {
            continue;
        }

        // Builds new path
        strcpy(new_path, curr_path);
        strcat(new_path, "/");
        strcat(new_path, d_entry->d_name);

        // Checks if directory
        if(d_entry->d_type == 4) {
            // Attempts to open subdirectory
            DIR* subdir = opendir(new_path);

            // Checks for error opening
            if(!subdir) {
                fprintf(stderr, "ERROR: Cannot open directory %s\nError Code %i: %s\n", new_path, errno, strerror(errno));
            } else {
                // Recursively searches subdirectory
                getStats(subdir, new_path, inode_counts, f_size_sum, alloc_blk_sum, 
                            num_two_plus_links, num_inval_links, num_prob_dir);

                // Closes subdirectory & checks for error
                if(closedir(subdir) == -1) {
                    fprintf(stderr, "ERROR: Cannot close directory %s\nError Code %i: %s\n", new_path, errno, strerror(errno));
                }

                // Checks if problematic (keyboard) directory
                for(int i = 0; i < d_entry->d_namlen; i++) {
                    if(isProblematic((d_entry->d_name)[i])) {
                        (*num_prob_dir)++;
                        break;
                    }
                }
            }
        }

        // Checks if file
        else if(d_entry->d_type == 8) {
            // Gets file stats
            if(stat(new_path, &entry_stats) == -1) {
                fprintf(stderr, "ERROR: Cannot get stats of file %s\nError Code %i: %s\n", new_path, errno, strerror(errno));
            } else {
                // Updates stat counters
                *f_size_sum += entry_stats.st_size;
                *alloc_blk_sum += entry_stats.st_blocks;

                // Checks if 2+ links
                if(entry_stats.st_nlink > 1) {
                    (*num_two_plus_links)++;
                }
            }
        }

        // Checks if symlink
        else if(d_entry->d_type == 10) {
            if(stat(new_path, &entry_stats) == -1 && errno == ENOENT){
                (*num_inval_links)++;
            } 
            // Checks if 2+ links
            else if(entry_stats.st_nlink > 1) {
                (*num_two_plus_links)++;
            }
        }
        // If other type of inode, check if 2+ links
        else {
            if(stat(new_path, &entry_stats) == -1) {
                fprintf(stderr, "ERROR: Cannot get stats of file %s\nError Code %i: %s\n", new_path, errno, strerror(errno));
            } else if(entry_stats.st_nlink > 1) {
                (*num_two_plus_links)++;
            }
        }

        // Adds 1 to inode type counter
        inode_counts[d_entry->d_type - 1]++;
    }
}


int main(int argc, char* argv[]) {

    // Variables for statistic tracking
    unsigned int inode_counts[15] = {0};  // Array to count each of 15 types of inodes (each index i is count for inode type i-1)
    unsigned int file_size_sum = 0;
    unsigned int allocated_block_sum = 0;
    unsigned int num_two_plus_links = 0;
    unsigned int num_invalid_symlinks = 0;
    unsigned int num_problematic_dir = 0;

    // Checks for argument
    char* d_path;
    if(argc != 2) {
        fprintf(stderr, "ERROR: Incorrect number of arguments.\nThere should be one argument, the starting file path.\n");
        return -1;
    } else {
        d_path = argv[1];
    }

    // Attempts to open directory
    DIR* dir = opendir(d_path);

    // Checks for error opening
    if(!dir) {
        fprintf(stderr, "ERROR: Cannot open directory %s\nError Code %i: %s\n", d_path, errno, strerror(errno));
    } else {
        // Recursively searches directory
        getStats(dir, argv[1], inode_counts, &file_size_sum, &allocated_block_sum, 
                &num_two_plus_links, &num_invalid_symlinks, &num_problematic_dir);

        // Closes directory & checks for error
        if(closedir(dir) == -1){
            fprintf(stderr, "ERROR: Cannot close directory %s\nError Code %i: %s\n", d_path, errno, strerror(errno));
        }
    }

    // ----- Prints collected stats -----
    printf("\n Discovered Stats\n");
    printf("--------------------\n\n");
    
    // Prints counts of inode types
    for(int i = 0; i < 15; i++) {
        printf("%u inode(s) of type %i\n", inode_counts[i], i+1);
    }

    printf("\n%u total bytes for all files found.\n", file_size_sum);
    printf("%u total blocks allocated for all files found.\n", allocated_block_sum);
    printf("%u inode(s) with link count greater than 1.\n", num_two_plus_links);
    printf("%u invalid symbolic link(s).\n", num_invalid_symlinks);
    printf("%u problematic directories.\n", num_problematic_dir);

    return 0;
}
