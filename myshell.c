#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#define MAX 1024

void pipeline(char **task, int a, int c);
void execute(char **task, int b, int a);
void execute2(char *filee, char **commend, int a, int x, int b, int c);

void sigintt(int sig)
{
}

void signall()
{
    signal(SIGINT, sigintt);
}

int divide(char* task2, char** task1)
{
    int a = 0;
    char* b;
    char* c = " ";
    b = strtok(task2, c);
    while(1)
    {
        if(b != NULL)
        {
            task1[a++] = b;
            b = strtok(NULL, c);
        }
        else
        break;
    }
    task1[a] = NULL;
    return a;
}

void my_cd(char **task, char **diir)
{
    if(task[1] == NULL)
    {
        if(diir[1] != NULL)
        {
            diir[2] = diir[1];
            diir[0] = diir[2];
        }
        diir[1] = strdup(getenv("HOME"));
        printf("已切换到家目录\n");
    }
    else if(strcmp(task[1], "-") != 0 && strcmp(task[1], "~") != 0)
    {
        printf("1\n");
        if(diir[1] != NULL)
        {
            diir[2] = diir[1];
            diir[0] = diir[2];
        }
        diir[1] = task[1];
        printf("已切换到相应目录\n");
    }
    else if(strcmp(task[1], "~") == 0)
    {
        printf("1\n");
        if(diir[1] != NULL)
        {
            diir[2] = diir[1];
            diir[0] = diir[2];
        }
        diir[1] = strdup(getenv("HOME"));
        printf("已切换到家目录\n");
    }
    else if(strcmp(task[1], "-") == 0)
    {
        printf("1\n");
        if(diir[1] == NULL)
        printf("这是第一个目录\n");
        else
        {
            diir[2] = diir[1];
            diir[1] = diir[0];
            diir[0] = diir[2];
            printf("已切换到上一个目录\n");
            
        }
    }
    if(chdir(diir[1]) == -1)
    {
        printf("切换失败\n");
        perror("chdir");
    }
    else
    printf("%s\n",diir[1]);
}

void run(char **task, int a)
{
    pid_t pi;
    pi = fork();
    if(pi == -1)
    perror("fork");
    else if(pi == 0)
    {
        //printf("%d\n\n",a);//a right
        pipeline(task, a, 0);
        exit(0);
    }
    else
    waitpid(pi, NULL, 0);
}

void pipeline(char **task, int a, int c)
{
    //printf("pipi\n");
    if(a == c)
    return;
    int fdd[2];
    if(pipe(fdd) == -1)
    {
        perror("pipe");
    }
    int b = 1024;
    for(int i = c; i < a; i++)
    {
        //printf("%s\n",task[i]);
        if(strcmp(task[i], "|") == 0)
        {
            b = i;
            
            //printf("%d %s\n\n",b, task[b]);
            break;
        }
    }
    if(b == a-1)
    {
        printf("'|'后没有输入命令\n");
        return;
    }
    else if(b == 1024 && a >= 1)
    {
        //printf("111\n");
        execute(task, c, a);
        return;
    }
    pid_t pi;
    pi = fork();
    if(pi == -1)
    perror("fork");
    if(pi == 0)
    {
        //printf("222\n");
        close(fdd[0]);
        dup2(fdd[1], STDOUT_FILENO);
        //close(fdd[1]);
        execute(task, c, b);
        exit(0);
    }
    if(pi > 0)
    {
        waitpid(pi, NULL, 0);
        close(fdd[1]);
        dup2(fdd[0], STDIN_FILENO);
        //close(fdd[0]);
        pipeline(task, a, b+1);
    }

}

void execute(char **task, int b, int a)
{
    //printf("111\n");
    char *filein;
    char *fileout;
    char *filee;
    int x= 0;
    // char *command[MAX];

    // //printf("%d",a);
    // for(int i = b; i < a; i++)
    // {
    //     command[i] = task[i];
    //     //printf("%s %s\n", command[i], task[i]);
    // }
    // //printf("%s\n\n",task[a-1]);
    // command[a] = NULL;


    for(int i = b; i < a; i++)
    {
        if(strcmp(task[i], "&") == 0)
        {
            x = 1;
            task[i] = NULL;
            break;
        }
    }
    int c = 0;
    for(int i = b; i < a; i++)
    {
        if(strcmp(task[i], "<") == 0)
        {
            //printf("%s 123\n",task[i]);
            filein = task[i+1];
            task[i] = NULL;
            //printf("%s\n",task[i]);
            execute2(filein, task, 1, x, b, a);
            c = 1;
            return;
            //break;
        }

        if(strcmp(task[i], ">") == 0)
        {
            fileout = task[i+1];
            task[i] = NULL;
            execute2(fileout, task, 2, x, b, a);
            c = 1;
            return;
            //break;
        }

        if(strcmp(task[i], ">>") == 0)
        {
            filee = task[i+1];
            task[i] = NULL;
            execute2(filee, task, 3, x, b, a);
            c = 1;
            return;
            //break;
        }
    }
    if(c == 0)
    {
        //execute2(NULL, task, 4, x, b, a);
        
        //printf("aaa\n");
        //printf("%s\n",command[b]);
        char *command[MAX];
        //printf("%d",a);
        for(int i = b; i < a; i++)
        {
            command[i] = task[i];
            //printf("%s %s\n", command[i], task[i]);
        }
        //printf("%s\n\n",task[a-1]);
        command[a] = NULL;
        pid_t pid = fork();
        if(pid == -1)
        {
            perror("fork");
        }
        else if(pid == 0)
        {
            //printf("b\n");
            execvp(command[b], command + b);
            //printf("b\n");
            exit(0);
        }
        else if(pid > 0)
        {
            if(x  == 0)
            waitpid(pid, NULL, 0);
        }
    }
}

void execute2(char *filee, char **task, int a, int x, int b, int c)
{
    //printf("b\n");
    int fdd;
    int fdin, fdout;
    char *command[MAX];
    //printf("%d",a);
    for(int i = b; i < c; i++)
    {
        command[i] = task[i];
        //printf("%s %s\n", command[i], task[i]);
    }
    //printf("%s\n\n",task[a-1]);
    command[c] = NULL;
    if(a == 1)
    {
        //printf("345\n");
        fdd = open(filee, O_RDONLY);
        if (fdd == -1) {
            perror("open failed");
            exit(1);
        }
        dup2(fdd, STDIN_FILENO);
        close(fdd);
    }
    else if(a == 2)
    {
        //printf("456\n");
        fdd = open(filee, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fdd == -1) {
            perror("open failed");
            exit(1);
        }
        dup2(fdd, STDOUT_FILENO);
        close(fdd);
    }
    else if(a == 3)
    {
        //printf("567\n");
        fdd = open(filee, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fdd == -1) {
            perror("open failed");
            exit(1);
        }
        dup2(fdd, STDOUT_FILENO);
        close(fdd);
    }
    // else if(a == 4)
    // {
    //     printf("678\n");
    // }
    pid_t pid;
    pid = fork();
    if(pid == -1)
    {
        perror("fork");
        exit(1);
    }
    else if(pid == 0)
    {
        execvp(command[b], command+b);
        perror("execvp failed");
        exit(0);
    }
    else if(pid > 0)
    {
        if(x == 0)
        {
            waitpid(pid, NULL, 0);
        }
        else if(x == 1)
        {
        }
    }
}

int main()
{
    char **task = (char**)malloc(MAX*sizeof(char*));
    char **diir = (char**)malloc(MAX*sizeof(char*));
    char *ans[MAX];

    for(int i = 0; i < MAX; i++)
    {
        task[i] = (char*)malloc(MAX);
        diir[i] = (char*)malloc(MAX);
    }
    diir[0] = getcwd(diir[0], 1024);
    diir[1] = NULL;
    signall();
    while(1)
    {
        printf("\033[1;32mMyShell$\033[0m ");
        char *taskk = (char*)malloc(MAX);
        if(fgets(taskk, MAX, stdin) == NULL)
        {
            perror("fgets");
        }
        taskk[strcspn(taskk, "\n")] = '\0';
        if(strlen(taskk) == 0)
        {
            printf("没有输入命令\n");
            continue;
        }
        if(strcmp(taskk, "exit") == 0)
        break;
        int a = divide(taskk, task);
        if(strcmp(task[0], "cd") == 0)
        {
            my_cd(task, diir);
        }
        else
        run(task, a);
    }
}