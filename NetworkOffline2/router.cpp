#include <bits/stdc++.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

#include <string>
#include "RoutingInfo.h"
#include "ReceiveSocket.h"
#include "SendSocket.h"
#include "HashTable.h"

//let's define some macros
#define DRIVER_IP "192.168.10.100"
#define DEFAULT_PORT 4747




using namespace std;

///----------------------formalities aka prototypes--------------------


//-----
///---------------------global variables----------------------------------



class Router{
    ReceiveSocket* receiveSocket;
    StringHashTable<SendSocket>* sendSockets;
    StringHashTable<RoutingInfo>* routingTable;

    string myIp;
    unsigned short myPort;
    int sockfd;


    string driverIp;
    vector<string> neighbors;
    vector<string> allRouters;



public:

    Router(string myIp,unsigned short myPort, string driverIp, vector<string> allRouters, vector<pair<string, int> > neighborInfo){

        this->myIp = myIp;
        this->myPort = myPort;
        this->driverIp = driverIp;
        this->allRouters = allRouters;


        ///-----------------initialize the routing table-----------------------------

        routingTable = new StringHashTable<RoutingInfo>(61);

        for(int i=0; i<allRouters.size(); i++){

            string ip = allRouters[i];
            RoutingInfo* info = new RoutingInfo(ip);

            routingTable->put(ip, info);
        }

        ///----------------create sockets for neighbors----------------------------
        sendSockets = new StringHashTable<SendSocket>(17);

        receiveSocket = new ReceiveSocket(myIp, myPort);
        sockfd = receiveSocket->getSockfd();

        for(int i=0; i<neighborInfo.size(); i++){
            string ip = neighborInfo[i].first;
            SendSocket* sock = new SendSocket(sockfd, ip, myPort);

            cout<<ip<<" "<<sock<<endl;

            neighbors.push_back(ip);
            sendSockets->put(ip, sock);

            int distance = neighborInfo[i].second;
            routingTable->get(ip)->update(distance, ip);  ///update in routing table for the neighbor...
        }

        printRoutingTable();

    }


    void start(){


        while(true){
            Packet packet = receiveSocket->receivePacket();

            cout<<packet.getSenderIp()<<" "<<packet.getMessage()<<endl;

            if(packet.getSenderIp()==DRIVER_IP) {
               // cout<<"got message from driver"<<endl;
                followDriverInstruction(packet.getMessage());
            }
            else {
                followRouterInstruction(packet.getSenderIp(), packet.getMessage());
            }


        }

    }

    void followDriverInstruction(string message);
    void updateRoutingTable(Packet packet);
    void sendMessage(string ip, string message);
    void sendRoutingTableToNeighbors();
    void printRoutingTable();
    void followRouterInstruction(string sender, string message);

};


void Router::followDriverInstruction(string message){


    if(message[0]=='s'){
        if(message[1]=='e' && message[2]=='n' && message[3]=='d'){
            cout<<"Driver says send"<<endl;
            ///todo
        }
        else if(message[1]=='h' && message[2]=='o' && message[3]=='w'){
            cout<<"Driver says show"<<endl;
            printRoutingTable();
        }
        else {cout<<"Invalid message from driver";}
    }
    else if(message[0]=='c'){
        if(message[1]=='o' && message[2]=='s' && message[3]=='t'){
            cout<<"Driver says cost"<<endl;
           /// updateCostOfRouting
        }
        else if(message[1]=='l' && message[2]=='k'){
            cout<<"Driver says clk"<<endl;

            sendRoutingTableToNeighbors();
        }
        else {cout<<"Invalid message from driver";}
    }
    else {cout<<"Invalid message from driver";}

   // printRoutingTable();
//   sendMessage(allRouters[1], message);
}

void Router::followRouterInstruction(string sender, string message){
    if(message[0]=='r' && message[1]=='t'){

    }
}



void Router::updateRoutingTable(Packet packet){

}
void Router::sendMessage(string ip, string message){  ///this function is giving segmentation fault.. need to solve this.

    SendSocket* socket = sendSockets->get(ip);
    if(socket==null) {cout<<"socket not found\n"; return;}
   // cout<<ip<<" "<<socket<<endl;
    socket->sendMessage(message);  ///actually this is where we are getting segmentation fault
}

void Router::printRoutingTable(){
    cout<<"======================================"<<endl;
    cout<<"Routing table of "<<myIp<<": "<<endl;
    for(int i=0; i<allRouters.size(); i++){
        cout<<routingTable->get(allRouters[i])->toString()<<endl;
    }
    cout<<"======================================"<<endl;

}


void Router::sendRoutingTableToNeighbors(){

}





int main(int argc, char** argv){

	if(argc<3){
		cout<<"Try executing with "<<argv[0]<<" <IP> <topo>\n";
		exit(1);
	}


	vector<string> allRouters;
	vector< pair<string, int> > neighborInfo;

	///populating with some fake data-------------------------
	for(int i=0; i<10; i++){
        stringstream ss;
        ss<<"192.168.10."<<(i+2);
        string s = ss.str();

        allRouters.push_back(s);
       // if(i%3==0){
            neighborInfo.push_back(pair<string, int>(s, (i+7)%5));
        //}
	}

	///now time to start the router, it will do rest of the works--------------------------

	Router* router = new Router(argv[1], DEFAULT_PORT, DRIVER_IP,allRouters,neighborInfo);

	router->start();

	return 0;

}
