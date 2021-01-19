# Program 2 - Stat Checker

The purpose of this assignment was to write a program that recursively explores the filesystem starting from a given point, which is the sole argument to the 
program, and reports the following statistics:

  a) # of inodes of each type (e.g. directory, file, symlink, etc.) encountered.
  
  b) for all regular files encountered, the sum of their sizes, and the sum of the number of disk blocks allocated for them.
  
  c) # of inodes (except, of course, directories) which have a link count of more than 1.
  
  d) # of symlinks encountered that did not (at least at the time of the exploration) resolve to a valid target.
  
  e) # of directory entries encountered which would be "problematic" to enter at the keyboard. This means any name which contains non-printable characters, non-ASCII characters, or special characters that confuse the shell unless properly escaped.
