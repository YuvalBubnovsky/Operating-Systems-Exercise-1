#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// Server Connection Data
#define SERVER_PORT 6090
#define SERVER_IP_ADDRESS "127.0.0.1"

// Delimiter definition needed for parsing, this will be passed to strtok()
#define DELIM " \t\r\n\a"
#define TOKEN_SIZE 256

#define NUM_BUILT_IN 8

/*
Used this https://brennan.io/2015/01/16/write-a-shell-in-c/ as a strong reference
for how to write a proper shell, some basic functions are identical as we believe this
is the best way to implement them. Awesome blog post which summerizes shell quite nicely.
*/

char *builtin_str[] = {
    "echo",
    "tcp port",
    "local",
    "dir",
    "cd",
    "copy",
    "delete",
    "exit"};

int echo(char **args);
int tcp_port(char **args);
int local(char **args);
int dir(char **args);
int cd(char **args);
int copy(char **args);
int delete (char **args);
int shell_exit(char **args);

int (*builtin_func[])(char **) = {
    &echo,
    &tcp_port,
    &local,
    &dir,
    &cd,
    &copy,
    &delete,
    &shell_exit};

/*
  Builtin function implementations.
*/



int execute(char **args)
{
    if (args[0] == NULL)
    {
        return 1; // Empty command
    }

    for (int i = 0; i < NUM_BUILT_IN; i++)
    {
        if (strcmp(args[0], builtin_str[i]) == 0)
        {
            return (*builtin_func[i])(args);
        }
    }

    return system(args[0]); // if no built-in command is found, use system for it
                            // TODO: Check to see if there's a safer way to do this (depends on system return value)
}
char *read_command(void)
{
    char *input = NULL;   // we set to NULL so getline will allocate a buffer for storing the line
    size_t buff_size = 0; // no need to allocate an actual number here, getline allocates a size as needed

    if (getline(&input, &buff_size, stdin) == -1)
    { // getline takes care of all allocations and reallocations while trying to read a string from stdin, for more :https://man7.org/linux/man-pages/man3/getline.3.html
        if (feof(stdin))
        { // If we recieved EOF, exit gracefully
            exit(0);
        }
        else
        {
            perror("getline");
            exit(1);
        }
    }

    return input;
}

char **parse_args(char *input)
{
    int buff_size = TOKEN_SIZE;
    int pos = 0;

    char *token;
    char **tokens = malloc(sizeof(char *) * buff_size);
    if (tokens == NULL)
    {
        fprintf(stderr, "Couldn't Allocate For Tokens!\n");
        exit(1);
    }

    token = strtok(input, DELIM);
    while (token != NULL)
    {
        tokens[pos] = token;
        pos++;

        // It will probaly be a smart idea to implement an option to reallocate tokens here, but not needed in this assignment
        token = strtok(NULL, DELIM); // searching for more tokens in the string
    }
    tokens[pos] = NULL;
    return tokens;
}

void command_loop(void)
{
    char *commands;
    char **args;
    int flag = 1;

    while (flag)
    {
        printf("yes master?");
        commands = read_command();
        args = parse_args(commands);
        flag = execute(args);

        free(commands);
        free(args);
    }
}

int main()
{

    command_loop();

    return 0;
}