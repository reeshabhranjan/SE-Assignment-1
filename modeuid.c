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
    seteuid(0);
    sleep(10);
    return 0;
}