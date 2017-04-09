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

           // cout<<packet.getSenderIp()<<" "<<packet.getMessage()<<endl;

            if(packet.getSenderIp()==DRIVER_IP) {
               // cout<<"got message from driver"<<endl;
                followDriverInstruction(packet);
            }
            else {
                followRouterInstruction(packet.getSenderIp(), packet.getMessage());
            }


        }

    }

    void followDriverInstruction(Packet packet);
    void updateRoutingTable(Packet packet);
    void sendMessage(string ip, string message);
    void sendRoutingTableToNeighbors();
    void printRoutingTable();
    void followRouterInstruction(string sender, string message);

};


void Router::followDriverInstruction(Packet packet){

    string message = packet.getMessage();
    vector<unsigned char> bytes = packet.getBytes();
    //vector<string> params = split(message, ' ');
    if(startsWith(message,"send")){
        cout<<"Driver says send"<<endl;
        string ip1 = extractIpFromBytes(bytes, 4);
        string ip2 = extractIpFromBytes(bytes, 8);
        unsigned short msgLength = (unsigned short) extractIntFromBytes(bytes, 12, 2);
        string msg = extractStringFromBytes(bytes, 14, msgLength);

        cout<<ip1<<" "<<ip2<<" "<<msgLength<<" "<<msg<<endl;

    }
    else if(startsWith(message, "show")){
        cout<<"Driver says show"<<endl;
        cout<<extractIpFromBytes(bytes, 4)<<endl;
    }
    else if(startsWith(message, "cost")){

        cout<<"Driver says cost"<<endl;
        string ip1 = extractIpFromBytes(bytes, 4);
        string ip2 = extractIpFromBytes(bytes, 8);
        int cost = extractIntFromBytes(bytes, 12);
        cout<<ip1<<" "<<ip2<<" "<<cost<<endl;
    }
    else if(startsWith(message, "clk")){
        cout<<"Driver says "<<message<<endl;

    }

   // printRoutingTable();
//   sendMessage(allRouters[1], message);
}

void Router::followRouterInstruction(string sender, string message){

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
