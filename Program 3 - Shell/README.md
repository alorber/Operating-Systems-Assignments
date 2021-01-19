# Program 3 - Shell

The goal of this assignment was to write a simplistic UNIX shell capable of launching one program at a time, with arguments, and waiting for and reporting the 
exit status and resource usage statistics. The shell accepts lines of input from standard input until EOF. 

Each line of input is a command to be executed. For each line:
1) if the line begins with a #, ignore it.
2) split up the line into tokens and separate the I/O redirection tokens from the command and its arguments.
3) _fork_ to create a new process in which to run the command.
4) establish the requested I/O redirection (if any) in the child. If any I/O redirection fails, report the error message and exit the child with an exit status of 1.
5) _exec_ the command. If the _exec_ system call fails, report the error message and exit the child with an exit status of 127.
6) _wait_ for the command to complete.
7) report the following information about the command that was just spawned: 
    - exit status or signal which killed the process
    - real time elapsed
    - user and system time
8) remember the exit status, which will be needed by the exit built-in command.
9) If any errors are encountered at any of these steps, report the error, but do not quit; just go on to the next line of input.


**Syntax and Tokenization**

Each command is formatted as follows:

```command {argument {argument...} } {redirection_operation {redirection_operation...}}```

It can be assumed that the redirection operators (if any) will only appear after the command and arguments (if any) as illustrated above. 

**Built-in Commands**

Built-in commands do not result in the _forking_ and _exec_’ing, but are handled directly within your shell. 

The following built-in commands are supported:

1) ```cd {dir}```: Change the current working directory of the shell with the chdir system call. If the directory is not specified, use the value of the environment variable HOME.
2) ```pwd```: Display the current working directory.
3) ```exit {status}```: Exit your shell immediately, with the status value specified (an integer) as the return value from your shell. If no status is given, use the exit status of the last command which you spawned (or attempted to spawn). 

**I/O redirection**

The following redirection operations are supported:

1) ```<filename```: Open filename and redirect stdin
2) ```>filename```: Open/Create/Truncate filename and redirect stdout
3) ```2>filename```: Open/Create/Truncate filename and redirect stderr
4) ```>>filename``` Open/Create/Append filename and redirect stdout
5) ```2>>filename``` Open/Create/Append filename and redirect stderr

Note that a given command launch can have 0, 1, 2 or 3 possible redirection operators. 
A failure to establish any of the requested I/O redirections should result in an error message and the command should not be launched. 

