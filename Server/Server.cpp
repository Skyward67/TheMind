#include <iostream>
#include <thread>

#include <sys/time.h>
#include "Socket.h"

using namespace std;
using namespace stdsock;

void lobby(int client, int i);

int main(int argc, char *argv[]){

    int port;
    if(argc!=2 || sscanf(argv[1], "%d", &port)!=1)
    {
            printf("usage: %s port\n", argv[0]);
            // default port, if none provided
            port= 3490;
    }

    ConnectionPoint *server=new ConnectionPoint(port);
    int err= server->init();
    if (err != 0) {
        std::cout << strerror(err) << std::endl;
        exit(err);
    }

    cout << "Waiting clients on port " << port << " ..." << endl;
    int i = 0;
    while(1){
        StreamSocket *client1=server->accept();
        cout << "connexion client " << i <<"\n";
        std::thread t(lobby, client1->getSockfd(), i);
        t.detach();
        i++;
    }
    return 0;
}

void lobby(int client, int i){
    cout<<"client " << i << " " << client << " joined lobby \n";
}