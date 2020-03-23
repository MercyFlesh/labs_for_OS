#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <signal.h>
#include <errno.h>
#include <string.h>


int child_proc(char* args[])
{   
    errno = 0;
    printf("Child lab_4_3 pid: %d\n", getpid());
    execv(args[0], args);
    printf("Failed exec with error: %s\n", strerror(errno));
}


int main(int argc, char* argv[])
{
    pid_t pid;
    int status;
   
    void* child_stack = malloc(1024 * 1024);
    argv[0] = "lab_4_1";
    pid = clone(child_proc, (char*) child_stack + (1024 * 1024), SIGCHLD, argv);

    printf("Parent process lab_4_3 pid: %d\n", getpid());
    
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
    
    exit(0);
}