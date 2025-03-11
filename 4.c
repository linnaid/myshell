#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>


int main()
{
    char *a = getenv("HOME");
    printf("%s",a);

}