#include <iostream>
#include <thread>

#include <sys/time.h>
#include "Socket.h"

using namespace std;
using namespace stdsock;


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

    // accepting connexion
    // and preparing communication points
    StreamSocket *client1=server->accept();
    cout << "connexion client1\n";
    StreamSocket *client2=server->accept();
    cout << "connexion client2\n";

    // waiting end of communication
    printf("Deconnexion client1\n");
    delete(client1);

    printf("Deconnexion client2\n");
    delete(client2);

    // closing connexion point
    delete server;
    cout << "stop\n";
    return 0;

    
}