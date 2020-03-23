#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

struct th_args
{
    char flag;
};

int fd[2];


void* writer(void* args)
{
    printf("<--------------------- Started writer %u --------------------->\n", pthread_self());
    
    char* flag = &((struct th_args* ) args)->flag;
    char buf = 'A';

    while ((int *)(*flag) != 10)
    {
        write(fd[1], &buf, 1);
        if (buf == 'Z')
            buf = 'A' - 1;
        buf++;
        sleep(1);
    }

    printf("<--------------------- Ended writer %u --------------------->\n", pthread_self());
}


void* reader(void* args)
{
    printf("<--------------------- Started reader %u --------------------->\n", pthread_self());
    
    char* flag = &((struct th_args* ) args)->flag;
    errno = 0;
    char* buf;

    while ((int *)(*flag) != 10)
    {   
        if(read(fd[0], &buf, 1) != -1)
        {
            printf ("Reader: %c\n", buf);
        }
        else
        {
            printf("Failed read with error: %s\n", strerror(errno));
        }

        sleep(1);
    }

    printf("<--------------------- Ended reader %u --------------------->\n", pthread_self());
}




int main(int argc, char* argv[])
{
    struct th_args* params;
    pthread_t* th;

    const int num_channels = 2;
    params = malloc(num_channels * sizeof(struct th_args));
    th = malloc(num_channels * sizeof(pthread_t));

    pipe(fd);
    //fcntl(fd[0], F_SETFL, O_NONBLOCK);
    pipe2(fd, O_NONBLOCK);
    
    pthread_create(&th[0], NULL, writer, (void* )(&params[0]));
    pthread_create(&th[1], NULL, reader, (void* )(&params[1]));
    
    params[0].flag = getchar();
    params[1].flag = getchar();
    
    pthread_join(th[0], NULL);
    pthread_join(th[1], NULL);

    close(fd);
    return 0;
}