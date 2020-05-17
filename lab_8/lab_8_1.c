#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <mqueue.h>
#include <sys/mman.h>

#define MQ_MSGSIZE 1

const void close_data();


struct th_args
{
    char flag;
    mqd_t mq;                                                                                                                                                                                                                                                                                                                                                                 
};


void* th_func(void* args)
{
    char* flag = &((struct th_args* ) args)->flag;
    mqd_t mq = ((struct th_args* ) args)->mq;
    char buffer[MQ_MSGSIZE] = "A";
    
    while((int *)(*flag) != 10)
    {
        int res = mq_send(mq, buffer, 1, 0);
               
        sleep(1);
        
        if (res == -1) 
        {
            perror("Error mq_send");
        }
        else
        {
            printf("Send to queue: %c\n", buffer[res]);
            if (buffer[0] == "Z")
                memset(buffer, 'A' - 1, 1);
            memset(buffer, ++buffer[0], 1);
        }       
    }   
}


const void close_data(void* args)
{
    mq_close(((struct th_args* ) args)->mq);
    mq_unlink("/myqueue");
}


int main()
{
    errno = 0;
    struct mq_attr attr = {0, 10, MQ_MSGSIZE, 0, {0}};
    struct th_args params;
    pthread_t th;

    params.mq = mq_open("/myqueue", O_CREAT|O_WRONLY|O_NONBLOCK, 0644, &attr);

    if(pthread_create(&th, NULL, th_func, (void*)(&params)))
    {
        printf("Thread create failed with error: %s\n", strerror(errno));
        close_data((void* )&params);
        return -errno;
    }
       
    params.flag = getchar();

    void* status;
    int res = pthread_join(th, &status);
    if (res != 0)
    {
        printf("Thread join failed with error: %s\n", strerror(res));
        close_data((void* )&params);
        return -errno;
    }
    
    close_data((void* )&params);
    return 0;
}