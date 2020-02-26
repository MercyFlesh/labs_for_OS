#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



int main(int argc, char* argv[])
{
    pid_t pid;
    printf("Process lab_4_1 pid: %d\n", getpid());
    printf("Parent lab_4_1 pid: %d\n", getppid());

    if (argc >= 3 && argc <= 5)
    {
        for (int i = 1; i < argc; i++)
        {
            printf("%s\n", argv[i]);
            sleep(1);
        }       
    }

    switch(pid) 
    {
        case -1:
            exit(1);
        case 0:
            exit(37);
        default:
            exit(0);
    }
}