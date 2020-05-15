#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/types.h>


void* th_write(void* args);
void close_data();

char flag;                                                                                                                                                                                                                                                                                                                                                              
int fd; 


void sig_handler(int fd)
{
    printf("Reader fifo has been disabled\n");
}


void* th_open(void* args)
{
    errno = 0;
    
    while((int *)flag != 10)
    {
        fd = open("/tmp/my_fifo", O_CREAT|O_WRONLY|O_NONBLOCK);
        pthread_t th_w;
        
        if (fd == -1) 
        {
            perror("Error opening fifo");
            sleep(1);
        }
        else
        {
            if(pthread_create(&th_w, NULL, th_write, args))
            {
                printf("Thread create failed with error: %s\n", strerror(errno));
                close_data();
                return -errno;
            }

            pthread_join(th_w, NULL);            
        }
    }
}


void* th_write(void* args)
{
    char buf = 'A';

    while((int *)flag != 10)
    {
        write(fd, &buf, 1);
        printf("Writer: %c\n", buf);
        
        if (buf == 'Z')
            buf = 'A' - 1;
        buf++;

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
    signal(SIGPIPE, sig_handler);
    errno = 0;
    pthread_t th;

    mkfifo("/tmp/my_fifo", 0644);
    
    if(pthread_create(&th, NULL, th_open, NULL))
    {
        printf("Thread create failed with error: %s\n", strerror(errno));
        unlink("/tmp/my_fifo");
        return -errno;
    }
       
    flag = getchar();
    
    void* status;
    int res = pthread_join(th, &status);
    if (res != 0)
    {
        printf("Thread join failed with error: %s\n", strerror(errno));
        close_data();
        return -errno;
    }
    
    close_data();
    return 0;
}