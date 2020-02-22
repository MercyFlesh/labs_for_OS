#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

struct th_args
{
    long thread_id;
    char flag;
};

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

    pthread_exit(0);
}


int main(int argc, char* argv[])
{
    struct th_args params_1, params_2;
    pthread_t thread1, thread2;
    
    pthread_create(&thread1, NULL, th_func, (void* )(&params_1));
    pthread_create(&thread2, NULL, th_func, (void* )(&params_2));
    
    for (int i = 0; i < 10; i++)
    {
        printf("main thread:\t%u\titer: %d\n", pthread_self(), i);
        sleep(1);
    }

    params_1.flag = getchar();
    params_2.flag = getchar();
    

    int code_ex = pthread_join(thread1, NULL);
    printf("thread %u ended with code %u\n", params_1.thread_id, code_ex);
    code_ex = pthread_join(thread2, NULL);
    printf("thread %u ended with code %u\n", params_2.thread_id, code_ex);

    return 0;
}