#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


struct th_args
{
    int th_code;
    FILE* file;
    char flag;                                                                                                                                                                                                                                                                                                                                                                             
};

sem_t sem;

void* th_func(void* args)
{
    printf("<--------------------- Started thread %u --------------------->\n", pthread_self());
    sleep(3);

    int th_code = ((struct th_args* ) args)->th_code;
    FILE* file = ((struct th_args* ) args)->file;
    char* flag = &((struct th_args* ) args)->flag;

    while((int *)(*flag) != 10)
    {
        sleep(1);
        sem_wait(&sem);

        for (int i = 0; i <= 11; i++)
        {
            fwrite(&th_code, sizeof(int), 1, file);
            printf("%d\n", th_code);
            sleep(1);
        }

        sem_post(&sem);
        sleep(3);
    }

    printf("<--------------------- Ended thread %u --------------------->\n", pthread_self());
}


int main()
{
    struct th_args* params;
    pthread_t* th;

    const int num_threads = 2;
    params = malloc(num_threads * sizeof(struct th_args));
    th = malloc(num_threads * sizeof(pthread_t));

    FILE* file;
    file = fopen("some_file.txt", "w+");
    if (file != NULL)
    {
        params[0].th_code = 1;
        params[0].file = file;
        params[1].th_code = 2;
        params[1].file = file;

        sem_init(&sem, 0, 1);
        pthread_create(&th[0], NULL, th_func, (void*)(&params[0]));
        pthread_create(&th[1], NULL, th_func, (void*)(&params[1]));
        
        params[0].flag = getchar();
        params[1].flag = getchar();
        pthread_join(th[0], NULL);
        pthread_join(th[1], NULL);
        sem_close(&sem);
        fclose(file);
    }

    return 0;
}