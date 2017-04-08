#include <bits/stdc++.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

#include <string>
#include "RoutingTable.h"
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
    map <string, SendSocket*> sockets;
    RoutingTable table;

public:

    void followInstruction(string message);
    void updateRoutingTable(Packet packet);
    void sendMessage(string ip, string message);

};


void Router::followInstruction(string message){


}

void Router::updateRoutingTable(Packet packet){

}
void Router::sendMessage(string ip, string message){

    SendSocket* socket = sockets.find(ip)->second;
    socket->sendMessage(message);
}

int main(int argc, char** argv){

	if(argc<3){
		cout<<"Try executing with "<<argv[0]<<" <IP> <topo>\n";
		exit(1);
	}

	//Router* router = new Router(argv[1], DEFAULT_PORT);

	ReceiveSocket* mySocket = new ReceiveSocket(argv[1], DEFAULT_PORT);
	cout<<mySocket->getIp()<<" "<<mySocket->getPort()<<endl;
	//setupMySocketAndBind(argv[1]);

   // exit(0);
	while(true){
		Packet packet = mySocket->receivePacket();

		if(packet.getSenderIp()==DRIVER_IP) {
			cout<<"got message from driver"<<endl;
			//followInstruction(message);
		}

		cout<<packet.getSenderIp()<<" "<<packet.getMessage()<<endl;
		//cout<<<<endl;

		//if(strcmp(buffer,"shutdown")==0)break;

	}

	return 0;

}
