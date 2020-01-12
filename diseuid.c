#include<stdio.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<pwd.h>
#include<grp.h>
#include<string.h>

int main(int argc, char **argv)
{
    printf("euid: %d\n", geteuid());
    return 0;
}