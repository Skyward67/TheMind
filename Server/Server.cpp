#define SEPARATOR '/'

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
string createChannel(int nb_player, string name, StreamSocket* client);
string joinChannel(string channelName, StreamSocket* client);
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

vector<string> split(string txt, char separator = SEPARATOR){
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
                    client->send(createChannel(stoi(msgSplit.at(1)), msgSplit.at(2), client));
                }
                else if (msgSplit.at(0) == "JOIN"){
                    client->send(joinChannel(msgSplit.at(1), client));
                }
            }
            else if (msgSplit.size() == 2){
                if (msgSplit.at(0) == "JOIN"){
                    string message = "LIST";
                    message += SEPARATOR;
                    for(auto filename : getChannels()){
                        message += filename + SEPARATOR;
                    }
                    cout<< message << endl;
                    client->send(message);
                }
            }
        }

        msg = "";
    }
}

string createChannel(int nb_player, string name, StreamSocket* client){
    fstream file;

    file.open("Channels/" + name + ".txt",ios::out);

    if (!file){
        cout << "error in creating channel " << name << " : file not opened.";
        return string("ERRO" + SEPARATOR + '1' + SEPARATOR + '\n');
    }

    client->setChannelName(name);

    file << nb_player << endl;
    file << client << endl;

    file.close();

    return string("OK" + SEPARATOR + '\n');
}

string joinChannel(string channelName, StreamSocket* client){

    // vérification qu'il reste de la place dans le channel
    ifstream fluxRead("Channels/" + channelName + ".txt");
    if (fluxRead){
        int nbClientAccept = 0, nbClientCurrent = 0;
        string ligne;
        getline(fluxRead, ligne);
        nbClientAccept = stoi(ligne);
        while(getline(fluxRead, ligne)){
            if (ligne.compare("") != 0)
                nbClientCurrent++;
        }
        if(nbClientAccept == nbClientCurrent){
            return"ERRO" + SEPARATOR + '4' + SEPARATOR + '\n'; //no client more
        }
    }
    else{
        return "ERRO" + SEPARATOR + '1' + SEPARATOR + '\n';
    }

    // ajout du client dans le channel
    fstream file;

    file.open("Channels/" + channelName + ".txt",ios::app);

    if (!file){
        cout << "error in joining channel " << channelName << " : file doesn't exist.";
        return "ERRO" + SEPARATOR + '1' + SEPARATOR + '\n';
    }

    client->setChannelName(channelName);

    file << client << endl;

    file.close();

    return "OK" + SEPARATOR + '\n';
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