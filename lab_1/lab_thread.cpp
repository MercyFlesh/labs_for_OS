#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

void first()
{
    for (int i = 0; i < 10; i++)
    {
        cout << "fist id: " << this_thread::get_id() << endl;
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

void second()
{
    for (int i = 0; i < 10; i++)
    {
        cout << "second id: " << this_thread::get_id() << endl;
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

int main()
{
    thread th(first);
    thread g(second);

    th.join();
    g.join();
    return 0;
}
