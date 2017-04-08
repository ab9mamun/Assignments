#include <bits/stdc++.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

#include <string>
#include "RoutingInfo.h"
#include "ReceiveSocket.h"
#include "SendSocket.h"

//let's define some macros
#define DRIVER_IP "192.168.10.100"
#define DEFAULT_PORT 4747




using namespace std;

///----------------------formalities aka prototypes--------------------


//-----
///---------------------global variables----------------------------------



class Router{
    ReceiveSocket* receiveSocket;
    map<string, SendSocket*> sendSockets;
    map<string, RoutingInfo*> routingTable;

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
        for(int i=0; i<allRouters.size(); i++){

            string ip = allRouters[i];
            RoutingInfo* info = new RoutingInfo(ip);

            routingTable.insert(pair<string, RoutingInfo*> (ip, info));
        }

        ///----------------create sockets for neighbors----------------------------
        receiveSocket = new ReceiveSocket(myIp, myPort);
        sockfd = receiveSocket->getSockfd();

        for(int i=0; i<neighborInfo.size(); i++){
            string ip = neighborInfo[i].first;
            SendSocket* sock = new SendSocket(sockfd, ip, myPort);

            neighbors.push_back(ip);
            sendSockets.insert(pair<string, SendSocket*>(ip, sock));

            int distance = neighborInfo[i].second;
            routingTable.find(ip)->second->update(distance, ip);  ///update in routing table for the neighbor...
        }

        printRoutingTable();

    }


    void start(){


        while(true){
            Packet packet = receiveSocket->receivePacket();

            if(packet.getSenderIp()==DRIVER_IP) {
                cout<<"got message from driver"<<endl;
                followInstruction(packet.getMessage());
            }

            cout<<packet.getSenderIp()<<" "<<packet.getMessage()<<endl;
        }

    }

    void followInstruction(string message);
    void updateRoutingTable(Packet packet);
    void sendMessage(string ip, string message);
    void sendRoutingTableToNeighbors();
    void printRoutingTable();

};


void Router::followInstruction(string message){

    printRoutingTable();
}


void Router::updateRoutingTable(Packet packet){

}
void Router::sendMessage(string ip, string message){

    SendSocket* socket = sendSockets.find(ip)->second;
    socket->sendMessage(message);
}

void Router::printRoutingTable(){
    cout<<"Routing table of "<<myIp<<": "<<endl;
    for(int i=0; i<allRouters.size(); i++){
        cout<<routingTable.find(allRouters[i])->second->toString()<<endl;
    }

}















int main(int argc, char** argv){

	if(argc<3){
		cout<<"Try executing with "<<argv[0]<<" <IP> <topo>\n";
		exit(1);
	}


	vector<string> allRouters;
	vector< pair<string, int> > neighborInfo;

	for(int i=0; i<10; i++){
        stringstream ss;
        ss<<"192.168.10."<<(i+2);
        string s = ss.str();

        allRouters.push_back(s);
        if(i%3==0){
            neighborInfo.push_back(pair<string, int>(s, (i+7)%5));
        }
	}

	Router* router = new Router(argv[1], DEFAULT_PORT, DRIVER_IP,allRouters,neighborInfo);

	router->start();

	//ReceiveSocket* mySocket = new ReceiveSocket(argv[1], DEFAULT_PORT);
	//cout<<mySocket->getIp()<<" "<<mySocket->getPort()<<endl;
	//setupMySocketAndBind(argv[1]);

   // exit(0);
/*	while(true){
		Packet packet = mySocket->receivePacket();

		if(packet.getSenderIp()==DRIVER_IP) {
			cout<<"got message from driver"<<endl;
			//followInstruction(message);
		}

		cout<<packet.getSenderIp()<<" "<<packet.getMessage()<<endl;
		//cout<<<<endl;

		//if(strcmp(buffer,"shutdown")==0)break;

	}
	*/

	return 0;

}
