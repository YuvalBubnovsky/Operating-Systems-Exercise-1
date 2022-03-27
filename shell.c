// terminal shell

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{

    char *cmd;
    char *buffer[1048];
    char cwd[256];
    int n;
    int i;

    while (1)
    {
       // printf("~@");  greyed out to allow us to display the current working directory
        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
            perror("getcwd() error");
        }
        else
        {
            printf("%s:~@ ", cwd);
        }
        cmd = (char *)malloc(sizeof(char) * 1048);
        scanf("%s", cmd);
        n = 0;
        buffer[n] = strtok(cmd, " ");
        while (buffer[n] != NULL)
        {
            buffer[++n] = strtok(NULL, " ");
        }
        if (strcmp(buffer[0], "exit") == 0)
        {
            exit(0);
        }
        else if (strcmp(buffer[0], "ls") == 0)
        {
            system("ls");
        }
        else if (strcmp(buffer[0], "cd") == 0)
        {
            if (buffer[1] == NULL)
            {
                printf("No directory specified\n");
            }
            else
            {
                chdir(buffer[1]);
            }
        }
        else if (strcmp(buffer[0], "pwd") == 0)
        {
            system("pwd");
        }
        else if (strcmp(buffer[0], "cat") == 0)
        {
            if (buffer[1] == NULL)
            {
                printf("No file specified\n");
            }
            else
            {
                system("cat ");
            }
        }
        else if (strcmp(buffer[0], "echo") == 0)
        {
            if (buffer[1] == NULL)
            {
                printf("No text specified\n");
            }
            else
            {
                for (i = 1; i < n; i++)
                {
                    printf("%s ", buffer[i]);
                }
                printf("\n");
            }
        }
        else if (strcmp(buffer[0], "help") == 0)
        {
            printf("exit\n");
            printf("ls\n");
            printf("cd\n");
            printf("pwd\n");
            printf("cat\n");
            printf("echo\n");
            printf("help\n");
        }
        else
        {
            printf("Invalid command\n");
        }
    }

    return 0;
};