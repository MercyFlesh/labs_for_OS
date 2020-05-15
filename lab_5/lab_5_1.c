#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

void close_data(void* args);

struct th_args
{
    int th_code;
    FILE* file;
    char flag;                                                                                                                                                                                                                                                                                                                                                                
};

sem_t* sem;  

void* th_func(void* args)
{
    errno = 0;
    char* flag = &((struct th_args* ) args)->flag;
    FILE* file = ((struct th_args* ) args)->file;

    while((int *)(*flag) != 10)
    {
        sleep(1);
        if(!sem_wait(sem))
        {
            for (int i = 0; i <= 5; i++)
            {
                printf("%c", '1');
                fflush(stdout);
                fputc('1', file);
                fflush(file);
                sleep(1);
            }
            
            if (sem_post(sem) == -1)
            {
                printf("Failed to unlock semaphore whith error: %s\n", strerror(errno));
                close_data(args);
                exit(EXIT_FAILURE);
            }
        }
        
        sleep(1);
    }
}

void close_data(void* args)
{
    close(((struct th_args* ) args)->file);
    sem_close(sem);
    sem_unlink("semaphore_1");
}

int main()
{
    errno = 0;
    struct th_args params;
    pthread_t th;

    params.file = fopen("file.txt", "a+");
    sem = sem_open("semaphore_1", O_CREAT, 0644, 1);
    void* status;

    if(pthread_create(&th, NULL, th_func, (void*)(&params)))
    {
        printf("Thread create failed with error: %s\n", strerror(errno));
        close_data((void*)&params);
        return -errno;
    }
       
    params.flag = getchar();
    
    
    int res = pthread_join(th, &status);
    if (res != 0)
    {
        printf("Thread join failed with error: %s\n", strerror(res));
        close_data((void*)&params);
        return -errno;
    }

    close_data((void*)&params);
    return 0;
}