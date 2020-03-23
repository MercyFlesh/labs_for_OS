#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(int argc, char* argv[])
{
    pid_t pid, w;
    errno = 0;
    int status;
    pid = fork(); 
    
    if (pid == 0)
    {
        printf("Child lab_4_2 pid: %d\n", getpid());
        argv[0] = "lab_4_1";
        execv(argv[0], argv);
        printf("Failed exec with error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Process lab_4_2 pid: %d\n", getpid());
        
        while(waitpid(pid, &status, WNOHANG) == 0)
        {
            sleep(1);
        }
        
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

    exit(0);
}