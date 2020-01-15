#include<stdio.h>
#include<unistd.h>

int main()
{
    printf("ruid: %d\n", getuid());
    printf("euid: %d\n", geteuid());
    printf("rguid: %d\n", getgid());
    printf("eguid: %d\n", getegid());
    printf("Hello world!\n");
}