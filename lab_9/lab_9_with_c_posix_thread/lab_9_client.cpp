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
#include <signal.h>
#include <vector>

using namespace std;


struct th_client_args
{
    char flag;
    int client_socket;
    struct sockaddr_in clientSockAddr;
};


void sig_handler(int fd)
{
    cout << "Client is crushed or disconnect with server" << endl;
    sleep(1);
    //throw runtime_error("Client is crushed or disconnect with server");
}


void* client_get(void* args)
{
    errno = 0;
    char* flag = &static_cast<th_client_args*>(args)->flag;
    int client_socket = static_cast<th_client_args*>(args)->client_socket;
    
    while((int)*flag != 10)
    {
        const int RCV_SIZE = 1024;
        vector<char> buf_message(RCV_SIZE);
        int reccount = recv(client_socket, &buf_message[0], RCV_SIZE,0);
         
        if (reccount == -1) 
        {
            perror("recv error");
            sleep(1);
        }
        else if (reccount == 0) 
        { 
            sleep(1);
        }
        else
        {
            for (const auto& symb : buf_message)
                cout << symb;
            cout << endl;
            sleep(1);
        }
    }
}


void* client_send(void* args)
{
    char* flag = &static_cast<th_client_args*>(args)->flag;
    int client_socket = static_cast<th_client_args*>(args)->client_socket;
    int count = 0;
    string buf = "Good day: ";
    
    while((int)*flag != 10)
    {
        vector<char> buf_request(buf.begin(), buf.end());
        for (const auto& symb : to_string(count))
            buf_request.push_back(symb);

        int sendcount = send(client_socket, &buf_request[0], buf_request.size(), MSG_DONTWAIT);

        if (sendcount == -1) 
        {
            perror("send error");
        }
        else
        {
            cout << "Send OK" << endl;
        }

        ++count;
        sleep(1);
    }
}


void* client_conn(void* args)
{
    char* flag = &static_cast<th_client_args*>(args)->flag;
    int client_socket = static_cast<th_client_args*>(args)->client_socket;
    struct sockaddr_in clientSockAddr = static_cast<th_client_args*>(args)->clientSockAddr;
    
    while((int)*flag != 10)
    {
        int result = connect(client_socket, (struct sockaddr*)&clientSockAddr, sizeof(clientSockAddr));

        if (result == -1) 
        {
            perror("connect error");
            sleep(1);
        }
        else
        {
            cout << "\nConnection established with server" << endl;

            pthread_t th_client_send, th_client_get;
            pthread_create(&th_client_send, NULL, client_send, args);
            pthread_create(&th_client_get, NULL, client_get, args);
            
            void* status_1;
            void* status_2;
            int res_send = pthread_join(th_client_send, &status_1);
            int res_get = pthread_join(th_client_get, &status_1);
            
        }
    }
}


int main()
{ 
    signal(SIGPIPE, sig_handler);
    pthread_t th_client_conn;
    th_client_args params;
    
    params.client_socket = socket(AF_INET, SOCK_STREAM,0);
    fcntl(params.client_socket,F_SETFL,O_NONBLOCK);
    params.clientSockAddr.sin_family = AF_INET;
    params.clientSockAddr.sin_port = htons(5000);
    params.clientSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    cout << "Client started\n" << endl;

    pthread_create(&th_client_conn, NULL, client_conn, (void*)(&params));
    params.flag = getchar();
    
    void* status;
    int res = pthread_join(th_client_conn, &status);
       
    cout << "\nClient finished" << endl;
    close(params.client_socket);
    return 0;
}