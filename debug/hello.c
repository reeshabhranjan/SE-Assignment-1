#include<stdio.h>
#include<unistd.h>

int main()
{
    printf("%d\n", geteuid());
    printf("Hello world!\n");
}