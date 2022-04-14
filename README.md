# Operating Systems Exercise 1 - Shell in C

## How To Run:
firstly clone this repository to a convenient folder, after which you should open it using an IDE or navigate your CMD to that location and run the commands
``` make all ``` and then ``` ./server ``` to get the TCP server up & running followed by ```./shell``` which will execute our shell.

## Supported Built-Ins:
echo, local, dir, cd, copy, delete, exit & also a custom tcp_port, local commands.
* echo [input] - echoes the input the stdout
* dir [foldername] - lists all directories and files in current working directory
* cd [foldername] - changes current working directory
* copy [src][dest] - copies file from source to destination
* delete [filename] - deletes the specified file
* tcp_port - opens a connection to the TCP server and redirects all output to said server
* local - closes the connections with TCP server and brings output back to local machine
* exit - exits the shell
