#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#include <stdio.h>
#include <dirent.h>

#include <sys/time.h>
#include "Socket.h"

using namespace std;
using namespace stdsock;

void lobby(StreamSocket* client, int i);
void createChannel(int nb_player, string name, StreamSocket* client);
void joinChannel(string channelName, StreamSocket* client);
void update(StreamSocket* client);
vector<string> getChannels();

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
            msgSplit = split(msg);
            if (msgSplit.size() > 2){
                if (msgSplit.at(0) == "CRTE"){
                    createChannel(stoi(msgSplit.at(1)), msgSplit.at(2), client);
                }
                else if (msgSplit.at(0) == "JOIN"){
                    joinChannel(msgSplit.at(1), client);
                }
                client->send("OK");
            }
            else if (msgSplit.size() == 2){
                if (msgSplit.at(0) == "JOIN"){
                    string message = "LIST;";
                    for(auto filename : getChannels()){
                        message += filename + ";";
                    }
                    client->send(message + "\n");
                }
            }
        }

        msg = "";
    }
}

void createChannel(int nb_player, string name, StreamSocket* client){
    fstream file;

    file.open("Channels/" + name + ".txt",ios::out);

    if (!file){
        cout << "error in creating channel " << name << " : file not opened.";
        return ;
    }

    file << name << endl;
    file << nb_player << endl;
    file << client << endl;

    file.close();
}

void joinChannel(string channelName, StreamSocket* client){
    fstream file;

    file.open("Channels/" + channelName + ".txt",ios::app);

    if (!file){
        cout << "error in joining channel " << channelName << " : file doesn't exist.";
        return ;
    }

    file << client << endl;

    file.close();
}

vector<string> getChannels(){
    vector<string> output;
    
    DIR *d;
    struct dirent *dir;
    d = opendir("Channels/");
    if(d){
        string currentfile = "";
        while((dir = readdir(d)) != NULL){
            currentfile = split(dir->d_name,'.').at(0);
            if (currentfile.compare("") != 0){
                output.push_back(currentfile);
            }
        }
        closedir(d);
    }

    return output;
}