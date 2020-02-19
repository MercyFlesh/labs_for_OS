#include <iostream>
#include <unistd.h>
#include <pthread.h>

using namespace std;


void* threadOneFunct(void* args)
{
    cout << "Started thread\t" << pthread_self() << endl;
    *((long*) args) = pthread_self();
    
    for (int i = 0; i < 10; i++)
    {
        cout << "thread id:\t" << pthread_self() << "\titer: " << i << endl;
        sleep(3);
    }
    
    pthread_exit(0);
}


void* threadTwoFunct(void* args)
{
    cout << "Started thread\t" << pthread_self() << endl;
    *((long*) args) = pthread_self();

    for (int i = 0; i < 10; i++)
    {
        cout << "thread id:\t" << pthread_self() << "\titrer: " << i << endl; 
        sleep(3);
    }

    pthread_exit(0);
}


int main(int argc, char* argv[])
{
    pthread_t thread1, thread2;
    long thread_id_1;
    long thread_id_2;
    pthread_create(&thread1, NULL, threadOneFunct, &thread_id_1);
    pthread_create(&thread2, NULL, threadTwoFunct, &thread_id_2);
    
    for (int i = 0; i < 10; i++)
    {
        cout << "main thread:\t" << pthread_self() << "\titer: " << i << endl;
        sleep(1);
    }
    
    int code_ex = pthread_join(thread1, NULL);
    cout << "thread " << thread_id_1 << " ended with code " << code_ex << endl;
    code_ex = pthread_join(thread2, NULL);
    cout << "thread " << thread_id_2 << " ended with code " << code_ex << endl;

    return 0;
}