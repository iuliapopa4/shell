# shell
command-line utility that implements basic functionalities such as removing files and directories (rm command), moving files and directories (mv command), and listing directory contents (dir command). It also supports handling pipes in commands.

This project is a command-line utility that implements various shell commands such as rm, mv, dir, and handling pipes (|) for executing multiple commands. The program is written in C and uses several libraries including stdio.h, stdlib.h, string.h, unistd.h, readline/readline.h, readline/history.h, sys/wait.h, sys/stat.h, dirent.h, stdbool.h, errno.h, fcntl.h, sys/ioctl.h, and ctype.h.

The rm command is used to remove files and directories. It supports options like -i for interactive mode and -v for verbose mode. It also handles the removal of directories recursively using the -r option.

The mv command is used to move or rename files and directories. It supports options like -i for interactive mode and -S for creating a backup of the destination file with a specified suffix. It can also move multiple files to a directory specified by the -t option.

The dir command is used to list the files and directories in a given directory. It supports specifying the directory path as an argument. The command displays the sorted list of files and directories in a formatted manner.

The program also provides support for handling pipes (|) to execute multiple commands in succession. It splits the input into individual commands and uses fork() and execvp() to create child processes and execute the commands.

The program provides a command prompt (>) where users can enter commands to be executed. The command prompt supports command history using the readline library.
