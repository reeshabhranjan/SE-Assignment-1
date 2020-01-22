#include<stdio.h>
#include<string.h>
#include<stdlib.h>

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
    char** command1_args = (char **)(malloc(sizeof(char*) * (pipe_position)));
    for (int i = 0; i < pipe_position; i++)
    {
        // command1_args[i] = (char *)(malloc(sizeof(char) * (strlen(argv[i]) + 1)));
        // strcpy(command1_args[i], argv[i]);
        command1_args[i] = strdup(argv[i]);
    }

    // int arr_size = sizeof(command1_args) / sizeof(command1_args[0]);
    int arr_size = pipe_position;

    for (int i = 0; i < arr_size; i++)
    {
        printf("%s ", command1_args[i]);
    }

    printf("\n");
    
}
