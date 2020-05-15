#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>


void close_data();

char flag; 
int fd;                                                                                                                                                                                                                                                                                                                                                               


void* th_read(void* args)
{
    errno = 0;
    char* buf;

    while((int *)flag != 10)
    {
        buf = '\0';
        if (read(fd, &buf, 1) > 0)
        {
            printf("Reader: %c\n", buf);
        }
        else
        {
            printf("reading fifo is empty\n");
        }

        sleep(1);
    }   
}


void close_data()
{
    close(fd);
    unlink("/tmp/my_fifo");
}


int main()
{
    errno = 0;
    pthread_t th_r;

    mkfifo("/tmp/my_fifo", 0644);
    
    fd = open("/tmp/my_fifo", O_CREAT|O_RDONLY|O_NONBLOCK);

    if(pthread_create(&th_r, NULL, th_read, NULL))
    {
        printf("Thread create failed with error: %s\n", strerror(errno));
        close_data();
        return -errno;
    } 
       
    flag = getchar();

    void* status;
    int res = pthread_join(th_r, &status);
    if (res != 0)
    {
        printf("Thread join failed with error: %s\n", strerror(res));
        close_data();
        return -errno;
    }
    
    close_data();
    return 0;
}