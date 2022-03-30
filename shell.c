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

char *func_names[] = {
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

int (*func_implements[])(char **) = {
    &echo,
    &tcp_port, // TODO: implement
    &local,    // TODO: implement
    &dir,
    &cd,
    &copy,
    &delete,
    &shell_exit};

/*
  Builtin function implementations.
*/

int echo(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "shell error: expected argument to \"echo\"\n");
    }
    else
    {
        printf("ECHOING: %s\n", args[1]);
    }
    return 1;
}

int tcp_port(char **args)
{
    return 1; // placeholder
}

int local(char **args)
{
    return 1; // placeholder
}

// Thanks to https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
int dir(char **args)
{

    DIR *dir_p = opendir("."); // returns NULL on error
    if (dir_p == NULL)
    {
        fprintf(stderr, "Error opening directory pointer");
    }
    else
    {
        struct dirent *file_p;
        while ((file_p = readdir(dir_p)) != NULL)
        {
            printf("| %s |", file_p->d_name);
        }
        printf("\n");
        closedir(dir_p);
    }

    return 1;
}

int cd(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "shell error: expected argument to \"cd\"\n");
    }
    else
    {
        if (chdir(args[1]) != 0) // on chdir succuess, 0 is returned
        {
            perror("chdir");
        }
    }
    return 1;
}

int copy(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "shell error: expected argument to \"copy\"\n");
    }
    else
    {
        FILE *src = fopen(args[1], "r");
        FILE *dest = fopen(args[2], "w+");
        if (src == NULL || dest == NULL)
        {
            perror("fopen");
        }
        char buffer[256];
        int data;
        while ((data = fread(buffer, 1, 256, src)) != 0)
        {
            fwrite(buffer, 1, data, dest);
        }
        fclose(src);
        fclose(dest);
    }
    return 1;
}

int delete (char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "shell error: expected argument to \"delete\"\n");
    }
    else
    {
        if (unlink(args[1]) != 0)
        {
            perror("unlink");
        }
    }
    return 1;
}

int shell_exit(char **args)
{
    return 0;
}

int system_call(char **args)
{
    pid_t pid;
    int flag;

    pid = fork();
    if (pid == 0)
    {
        if (execvp(args[0], args) == -1)
        {
            perror("shell");
        }
        exit(1);
    }
    else if (pid < 0)
    {
        perror("shell");
    }
    else
    {
        // https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-waitpid-wait-specific-child-process-end
        // WIFEXITED - queries the child termination status provided by the wait and waitpid functions, and determines whether the child process ended normally.
        // WIFSIGNALED - queries the child termination status provided by the wait and waitpid functions. It determines if the child process exited because it raised a signal that caused it to exit
        // WUNTRACED - Reports on stopped child processes as well as terminated ones
        while (!WIFEXITED(flag) && !(WIFSIGNALED(flag)))
        {
            waitpid(pid, &flag, WUNTRACED);
        }
    }
    return 1;
}

int execute(char **args)
{
    if (args[0] == NULL)
    {
        return 1; // Empty command
    }

    for (int i = 0; i < NUM_BUILT_IN; i++)
    {
        if (strcmp(args[0], func_names[i]) == 0)
        {
            return (*func_implements[i])(args);
        }
    }

    // return system(args[0]); // if no built-in command is found, use system for it
    return system_call(args);
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
    char *cwd = (char *)malloc(2048);

    while (flag)
    {
        // printf("yes master?");
        getcwd(cwd, 2047);
        printf("%s~$ ", cwd);
        commands = read_command();
        args = parse_args(commands);
        flag = execute(args);

        free(commands);
        free(args);
    }
    free(cwd);
}

int main()
{

    command_loop();

    return 0;
}