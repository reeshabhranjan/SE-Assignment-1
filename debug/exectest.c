#include<stdio.h>
#include<unistd.h>

int main()
{
    char *arg[] = {"ls", "-l", "-a", NULL};
    execvp(arg[0], arg);
    return 0;
}