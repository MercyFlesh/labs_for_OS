#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <thread>
#include <chrono>
#include <vector>

using namespace std;


struct th_client_args
{
    char flag;
    int client_socket;
    struct sockaddr_in clientSockAddr;
};


void client_get(const th_client_args& args)
{
    while(args.flag != 10)
    {
        const int RCV_SIZE = 1024;
        vector<char> buf_message(RCV_SIZE);
        int reccount = recv(args.client_socket, &buf_message[0], RCV_SIZE,0);

        if (reccount == -1) 
        {
            perror("recv error");
            this_thread::sleep_for(chrono::seconds(1));
        }
        else if (reccount == 0) 
        {
            this_thread::sleep_for(chrono::seconds(1));
        }
        else
        {
            for (const auto& symb : buf_message)
                cout << symb;
            cout << endl;

            this_thread::sleep_for(chrono::seconds(1));
        }
    }
}


void client_send(const th_client_args& args)
{
    int count = 0;
    string buf = "Hello World: ";
    
    while(args.flag != 10)
    {
        vector<char> buf_request(buf.begin(), buf.end());
        for (const auto& symb : to_string(count))
            buf_request.push_back(symb);

        int sendcount = send(args.client_socket, &buf_request[0], buf_request.size(),0);
        if (sendcount == -1) 
        {
            perror("send error");
        }
        else
        {
            cout << "Send OK" << endl;
        }

        ++count;
        this_thread::sleep_for(chrono::seconds(1));
    }
}


void client_conn(th_client_args& args)
{
    while(args.flag != 10)
    {
        int result = connect(args.client_socket, (struct sockaddr*)&args.clientSockAddr, sizeof(args.clientSockAddr));

        if (result == -1) 
        {
            perror("connect error");
            this_thread::sleep_for(chrono::seconds(1));
        }
        else
        {
            cout << "\nConnection established with server" << endl;

            thread th_client_send(client_send, ref(args));
            thread th_client_get(client_get, ref(args));
        
            th_client_send.join();
            th_client_get.join();
        }
    }
}


int main()
{
    th_client_args params;
    
    params.client_socket = socket(AF_INET,SOCK_STREAM,0);
    fcntl(params.client_socket,F_SETFL,O_NONBLOCK);
    params.clientSockAddr.sin_family = AF_INET;
    params.clientSockAddr.sin_port = htons(5000);
    params.clientSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    cout << "Client started\n" << endl;

    thread th_client_conn(client_conn, ref(params));
    params.flag = getchar();
    th_client_conn.join();

    cout << "\nClient finished" << endl;
    close(params.client_socket);
    return 0;
}