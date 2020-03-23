#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>


struct th_args
{
    int th_code;
    char flag;                                                                                                                                                                                                                                                                                                                                                                             
};

sem_t sem;

void* th_func(void* args)
{
    printf("<--------------------- Started thread %u --------------------->\n", pthread_self());
    sleep(1);

    errno = 0;
    int th_code = ((struct th_args* ) args)->th_code;
    char* flag = &((struct th_args* ) args)->flag;

    while((int *)(*flag) != 10)
    {
        sleep(1);
        
        if(!sem_wait(&sem))
        {
            for (int i = 0; i <= 5; i++)
            {
                printf("%d\n", th_code);
                sleep(1);
            }
            
            if (sem_post(&sem) == -1)
            {
                printf("Failed to unlock semaphore whith error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
        
        
        sleep(1);
    }

    printf("<--------------------- Ended thread %u --------------------->\n", pthread_self());
}


int main()
{
    int ret;
    errno = 0;
    struct th_args* params;
    pthread_t* th;

    const int num_threads = 2;
    params = malloc(num_threads * sizeof(struct th_args));
    th = malloc(num_threads * sizeof(pthread_t));

    
    params[0].th_code = 1;
    params[1].th_code = 2;
    void* status_1;
    void* status_2;

    if (sem_init(&sem, 0, 1) != 0)
    {
        printf("Semaphore init failed with error: %s\n", strerror(errno));
        return -errno;
    }
    
    if(pthread_create(&th[0], NULL, th_func, (void*)(&params[0])))
    {
        printf("Thread create failed with error: %s\n", strerror(errno));
        return -errno;
    }
    
    if(pthread_create(&th[1], NULL, th_func, (void*)(&params[1])))
    {
        printf("Thread create failed with error: %s\n", strerror(errno));
        return -errno;
    }
        
    params[0].flag = params[1].flag = getchar();
    
    if (pthread_join(th[0], &status_2) != 0)
    {
        printf("Thread join failed with error: %s\n", strerror(errno));
        return -errno;
    }
    
    if (pthread_join(th[1], &status_2) != 0)
    {
        printf("Thread join failed with error: %s\n", strerror(errno));
        return -errno;
    }

    sem_close(&sem);

    return 0;
}