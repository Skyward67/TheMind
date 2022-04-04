#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#include <sys/time.h>
#include "Socket.h"

using namespace std;
using namespace stdsock;

void lobby(StreamSocket* client, int i);
void createChannel(int nb_player, string name);
void update(StreamSocket* client);

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
        std::thread t(lobby, client1, i);
        t.detach();
        std::thread u(update, client1);
        u.detach();
        i++;
    }
    return 0;
}

void lobby(StreamSocket* client, int i){
    //cout<<"client " << i << " " << client << " joined lobby \n";
    //string msg;
    //client->read(msg);
    //cout<<msg << "\n";
    //string message = "mon message";
    //client->send(message);
    //cout<<"asdsqfq\n";
}

vector<string> split(string txt, char separator = ';'){
    vector<string> output;
    stringstream streamData(txt);

    string val;

    while(getline(streamData, val, separator)){
        output.push_back(val);
    }

    return output;
}

void update(StreamSocket* client){
    string msg;
    vector<string> msgSplit;
    while(true){
        client->read(msg);

        if (msg != ""){
            cout << "read" << endl;
            msgSplit = split(msg);
            if (msgSplit.size() > 2){
                if (msgSplit.at(0) == "CRTE"){
                    cout << "CRTE" << endl;
                    createChannel(stoi(msgSplit.at(1)), msgSplit.at(2));
                }
                else if (msg == "JOIN"){
                    
                }

                client->send("OK");
            }
        }

        msg = "";
    }
}

void createChannel(int nb_player, string name){
    fstream file;

    file.open("Channels/" + name + ".txt",ios::out);

    if (!file){
        cout << "error in creating channel " << name << " : file not opened.";
        return ;
    }

    file << name << endl;
    file << nb_player << endl;

    file.close();
}