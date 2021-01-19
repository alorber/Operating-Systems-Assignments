# Program 5 - Smear

The purpose of this program was to take advantage of the _mmap_ system call to replace strings within given files. 

The program _smear_ is invoked by:
```
smear TARGET REPLACEMENT file1 {file 2....}
```

For each of the named files, it searches for every instance of the string given by the first argument and replace it with the replacement string given by the 
second argument. It is assumed that both the target and replacement strings are the same number of characters.
