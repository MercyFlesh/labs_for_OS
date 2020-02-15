#include <iostream>
#include <unistd.h>
#include <pthread.h>

using namespace std;

void* threadOneFunct(void* thread_data)
{
    for (int i = 0; i < 10; i++)
    {
        cout << "thread id:\t" << pthread_self() << endl;
        sleep(5);
    }
}

void* threadTwoFunct(void* thread_data)
{
    for (int i = 0; i < 10; i++)
    {
        cout << "thread id:\t" << pthread_self() << endl; 
        sleep(5);
    }
}

int main()
{
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, threadOneFunct, NULL);
    pthread_create(&thread2, NULL, threadTwoFunct, NULL);
    
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    return 0;
}