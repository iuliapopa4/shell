# shell
Personal implementation of shell commands such as rm, mv, dir, and handling pipes (|) for executing multiple commands. The program is written in C.
The rm command is used to remove files and directories. It supports options like -i for interactive mode and -v for verbose mode. It also handles the removal of directories recursively using the -r option.
The mv command is used to move or rename files and directories. It supports options like -i for interactive mode and -S for creating a backup of the destination file with a specified suffix. It can also move multiple files to a directory specified by the -t option.
The dir command is used to list the files and directories in a given directory. It supports specifying the directory path as an argument. The command displays the sorted list of files and directories in a formatted manner.
The program also provides support for handling pipes (|) to execute multiple commands in succession. It splits the input into individual commands and uses fork() and execvp() to create child processes and execute the commands.
The program provides a command prompt (>) where users can enter commands to be executed. The command prompt supports command history using the readline library.
