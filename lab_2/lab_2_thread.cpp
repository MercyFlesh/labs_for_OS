#include <thread>
#include <mutex>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

mutex mx;


void th_func(ifstream &file, int arg_code)
{
    cout << "<--------------------- Started thread " << this_thread::get_id() << "--------------------->" << endl;
    stringstream str_buf;
    
    while(!file.eof())
    {
        this_thread::sleep_for(chrono::seconds(3));
        mx.lock();
        
        for (int i = 0; (i <= 10) && (!file.eof()); i++)
        {
            str_buf << char(file.get()); 
            cout << arg_code << endl;
            this_thread::sleep_for(chrono::seconds(1));
        }

        mx.unlock();
        this_thread::sleep_for(chrono::seconds(3));
    }

    cout << "<--------------------- Ended thread " << this_thread::get_id() << "--------------------->" << endl;
}


int main(int argc, char* argv[])
{  
    ifstream file;
    file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    
    try
    {
        ifstream file("some_file.txt");
        thread th1(th_func, ref(file), 1);
        thread th2(th_func, ref(file), 2);
        
        th2.join();
        th1.join();
        file.close();
    }
    catch(const ifstream::failure &ex)
    {
        cout << ex.what() << endl;
        cout << ex.code() << endl;
    }
    catch(const exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}