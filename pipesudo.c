#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

int main(int argc, char **argv)
{
    int pipe_position = -1;
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "|") == 0)
        {
            printf("[.] comparing %s with |", argv[i]);
            pipe_position = i;
            break;
        }
    }

    printf("[.] pipe-position: %d\n", pipe_position);

    // OK

    char** command1_args = (char **)(malloc(sizeof(char*) * (pipe_position + 1)));
    char** command2_args = (char **)(malloc(sizeof(char*) * (argc - pipe_position)));
    for (int i = 0; i < pipe_position; i++)
    {
        command1_args[i] = strdup(argv[i]);
    }

    for (int i = pipe_position + 1; i < argc; i++)
    {
        command2_args[i] = strdup(argv[i]);
    }
    
    command1_args[pipe_position] = NULL;
    command2_args[argc - pipe_position - 1] = NULL;

    for (int i = 0; i < pipe_position + 1; i++)
    {
        printf("%s ", command1_args[i]);
    }
    printf("\n");

    for (int i = 0; i < argc - pipe_position; i++)
    {
        printf("%s ", command2_args[i]);
    }
    printf("\n");

    int fd[2];

    if (pipe(fd) == -1)
    {
        perror("Internal failure");
    }

    if (fork() == 0)
    {
        // first close stdout, close the reading end of the pipe 
        // and duplicate the writing end of the pipe
        close(1);
        dup(fd[1]);
        close(fd[0]);

        // char* arguments[] = {"/bin/ls"};
        execvp(command1_args[0], command1_args);
        // execvp(arguments[0], arguments);
    }
    else
    {
        // close stdin, close writing end of the pipe
        // and duplicate the reading end of the pipe
        close(0);
        dup(fd[0]);
        close(fd[1]);
        // char* arguments[] = {"/usr/bin/wc", "-l"};
        // execvp(arguments[0], arguments);
        execvp(command2_args[0], command2_args);
    }    
}