# Program 4 - A Three Command Launcher

The goal of this assignment was to build three separate programs (described below) and a fourth program to launch these three programs with a pipeline connecting 
them (in the order seen below). The launcher program takes one argument which is passed directly to wordgen. After launching the three child processes, the 
launcher program sits and waits for all children to exit, and reports the exit status of each child.

**wordgen:** 
Generates a series of random potential words, one per line, each randomly between 3 and 15 characters. 
If this command has an argument, that is the limit of how many potential words are generated. 
Otherwise, if the argument is 0 or missing, it generates words in an endless loop.
  
**wordsearch:**
Reads in a list of dictionary words, one per line, from a file that is supplied as the command-line argument. 
Then, reads a line at a time from standard input and determines if that potential word matches a word in the word list. 
If so, prints the word to standard output. The program continues until end-of-file on standard input. At the end of the program, 
wordsearch outputs to standard error the total number of words matched.
  
**pager:**
Reads from standard input and prints each line to standard output until 23 lines have been output. Then, it displays the message ```---Press RETURN for more---``` 
to the terminal and waits for a line of input to be received. Once a newline is received, the next set of 23 lines will be printed, until either end-of-file is 
seen on standard input, or the command q (or Q) is received while waiting for the next-page newline.
