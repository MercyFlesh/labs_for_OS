#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <queue>

using namespace std;


struct th_server_args
{
    char flag;
    int listen_socket;
    int conn_server;
    mutex mx;
    queue<string> querySet;
};


void server_send(th_server_args& args)
{
    while(args.flag != 10)
    {
        args.mx.lock();
        if(!args.querySet.empty())
        {
            string message = args.querySet.front();
            args.querySet.pop();
            args.mx.unlock();

            utsname uname_info;
            uname(&uname_info);

            string response = string(uname_info.sysname) + " " + 
                            string(uname_info.version) + " " + 
                            message;

            vector<char> resp_c(response.begin(), response.end());
            int sendcount = send(args.conn_server, &resp_c[0], resp_c.size(),0);

            if (sendcount == -1) 
            {
                perror("send error");
            } 
            else
            {
                cout << "Send OK" << endl;
            }
        }
        else
        {
            args.mx.unlock();
            this_thread::sleep_for(chrono::seconds(1));
        }   
    }
}


void server_get(th_server_args& args)
{
    const int RCV_SIZE = 1024;
    
    while(args.flag != 10)
    {
        vector<char> buf(RCV_SIZE);
        string rcv;
        int reccount = recv(args.conn_server, &buf[0], RCV_SIZE, NULL);
        
        if (reccount == -1) 
        {
            perror("recv error");
            sleep(1);
        }
        else if (reccount == 0) 
        {
            this_thread::sleep_for(chrono::seconds(1));
        }
        else
        {
            rcv.append(buf.begin(), buf.end());
            cout << rcv << endl;

            args.mx.lock();
            args.querySet.push(rcv);
            args.mx.unlock();
        }
    }
}


void server_conn(th_server_args& args)
{
    while(args.flag != 10)
    {
        args.conn_server = accept(args.listen_socket, (struct sockaddr*)NULL, NULL);

        if (args.conn_server == -1)
        {
            perror("accept error");
            this_thread::sleep_for(chrono::seconds(1));
        }
        else
        {
            cout << "\nClient connection" << endl;

            thread th_server_send(server_send, ref(args));
            thread th_server_get(server_get, ref(args));

            th_server_send.join();
            th_server_get.join(); 
        }
        
        shutdown(args.conn_server, 2);
        close(args.conn_server);
    }
}


int main()
{
    struct sockaddr_in servSockAddr;
    int optval = 1;
    th_server_args params;
    params.listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(params.listen_socket, F_SETFL, O_NONBLOCK);
    
    servSockAddr.sin_family = AF_INET;
    servSockAddr.sin_port = htons(5000);
    servSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    bind(params.listen_socket, (struct sockaddr*)&servSockAddr, sizeof(servSockAddr));
    setsockopt(params.listen_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));    
    listen(params.listen_socket, SOMAXCONN);
    cout << "Server started\n" << endl;
    
    thread th_server_conn(server_conn, ref(params));

    params.flag = getchar();

    th_server_conn.join();
    
    cout << "\nServer finished" << endl;
    close(params.listen_socket);
    return 0;
}