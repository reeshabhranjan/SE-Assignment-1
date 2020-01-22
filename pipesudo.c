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

    char* command1 = argv[0];
    char* command2 = argv[pipe_position + 1];
    char** command1_args = (char **)(malloc(sizeof(char*) * (pipe_position + 1)));
    for (int i = 0; i < pipe_position; i++)
    {
        // command1_args[i] = (char *)(malloc(sizeof(char) * (strlen(argv[i]) + 1)));
        // strcpy(command1_args[i], argv[i]);
        command1_args[i] = strdup(argv[i]);
    }

    command1_args[pipe_position] = NULL;

    int fd[2]; // 0 is the reading end and 1 is the writing end
    // in fd table, 0 is stdin, 1 is stdout, 2 is stderr

    if (pipe(fd) == -1)
    {
        perror("internal failure");
    }

    if (fork() == 0)
    {
        // first close stdout, close the reading end of the pipe 
        // and duplicate the writing end of the pipe
        close(1);
        dup(fd[1]);
        close(fd[0]);
        execvp(command1_args[0], command1_args);
    }
    else
    {
        // close stdin, close writing end of the pipe
        // and duplicate the reading end of the pipe
        close(0);
        dup(fd[0]);
        close(fd[1]);
        execvp(argv[pipe_position + 1], argv + pipe_position + 1);
    }
    
    
}
