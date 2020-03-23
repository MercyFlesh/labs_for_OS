#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

struct th_args
{
    long thread_id;
    char flag;
};

pthread_t thread1, thread2;

void* th_func(void* args)
{
    printf("Started thread\t%u\n",  pthread_self());
    sleep(3);
    long* thread_id = &((struct th_args* ) args)->thread_id;
    char* flag = &((struct th_args* ) args)->flag;
    *thread_id = pthread_self();
    
    int count = 0;
    while((int *)(*flag) != 10)
    {
        printf("thread id:\t%u\titrer: %d\n", pthread_self(), count);
        count++; 
        sleep(1);
    }
    if (pthread_self() == thread1)
        pthread_exit(5);
    else
        pthread_exit(6);
    
}


int main(int argc, char* argv[])
{
    struct th_args params_1, params_2;
    
    
    pthread_create(&thread1, NULL, th_func, (void* )(&params_1));
    pthread_create(&thread2, NULL, th_func, (void* )(&params_2));
    

    params_1.flag = params_2.flag = getchar();

    void* status_1;
    void* status_2;
    pthread_join(thread1, &status_1);
    printf("thread %u ended with code %u\n", params_1.thread_id, status_1);
    pthread_join(thread2, &status_2);
    printf("thread %u ended with code %u\n", params_2.thread_id, status_2);

    return 0;
}