#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char* argv[])
{
    pid_t pid, w;
    int status;
    pid = fork(); 
    
    if (pid == 0)
    {
        printf("Child lab_4_2 pid: %d\n", getpid());
        char* args[] = {"/home/noname/Документы/labs_c/lab_4_1", "4eburek", "anime", "kek", "lol"};
        execv(args[0], args);
        printf("lol\n");
    }
    else
    {
        printf("Process lab_4_2 pid: %d\n", getpid());
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) 
        {
            printf("Child exited, status=%d\n", WEXITSTATUS(status));
        } 
        else if (WIFSIGNALED(status)) 
        {
            printf("killed by signal %d\n", WTERMSIG(status));
        } 
        else if (WIFSTOPPED(status)) 
        {
            printf("stopped by signal %d\n", WSTOPSIG(status));
        } 
    }

    return 0;
}