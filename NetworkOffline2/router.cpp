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

           // cout<<ip<<" "<<sock<<endl;

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
                followRouterInstruction(packet);
            }


        }

    }

    bool dvrUpdate(string neighbor, string destination, int distance, string nextHop);

    void followDriverInstruction(Packet packet);
    void updateUsingNeighborsTable(string neighbor, vector<unsigned char> bytes, int whereToStart);
    void sendMessage(string ip, string message);
    void sendBytes(string ip, vector<unsigned char> bytes);
    void sendRoutingTableToNeighbors();
    void printRoutingTable();
    void followRouterInstruction(Packet packet);
    vector<unsigned char> extractBytesFromTable();


};






bool Router::dvrUpdate(string neighbor, string destination, int distance, string nextHop){

    RoutingInfo* info = routingTable->get(destination);
    int myOldDistance = info->getDistance();
    string myOldNextHop = info->getNextHop();


    int myNewDistance = routingTable->get(neighbor)->getDistance() + distance;

    if(myNewDistance>INF) myNewDistance = INF;

    if(myOldNextHop == neighbor ///***********important::: forced update
        || (myNewDistance < myOldDistance
            && myIp != nextHop) ///***********important::: split horizon
    ){

     info->update(myNewDistance, neighbor);
        return true;

    }
    return false;

}





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

        stringstream ss;
        ss<<"send "<<" "<<ip2<<" "<<msgLength<<" "<<msg<<endl;
        sendMessage(ip2, ss.str());

    }

    else if(startsWith(message, "show")){
        cout<<"Driver says show"<<endl;
        printRoutingTable();
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
        sendRoutingTableToNeighbors();
    }

   // printRoutingTable();
//   sendMessage(allRouters[1], message);
}

void Router::followRouterInstruction(Packet packet){
    string message = packet.getMessage();
    string sender = packet.getSenderIp();


    if(startsWith(message, "rt")){
        updateUsingNeighborsTable(sender, packet.getBytes(), 2);
    }

    else if(startsWith(message, "send")){
        cout<<sender<<" says send"<<endl;
    }

}



void Router::updateUsingNeighborsTable(string neighbor, vector<unsigned char> bytes, int whereToStart){

    int offset = whereToStart;

        int length = extractIntFromBytes(bytes, offset);
        offset+= 4;
       // cout<<length<<endl;
      //  cout<<"routing table of "<<neighbor<<":\n";
        for(int i=0; i<length; i++){
            string destination = getStringIp(extractIntFromBytes(bytes, offset));
            int distance = extractIntFromBytes(bytes, offset+4);
            string nextHop = getStringIp(extractIntFromBytes(bytes, offset+8));

            offset+=12;
            //cout<<destination<<" :: "<<distance<<" :: "<<nextHop<<endl;
           //dvrUpdate(neighbor, destination, distance, nextHop);
        }
}

void Router::sendMessage(string ip, string message){  ///this function is giving segmentation fault.. need to solve this.

    SendSocket* socket = sendSockets->get(ip);
    if(socket==null) {cout<<"socket not found\n"; return;}
   // cout<<ip<<" "<<socket<<endl;
    socket->sendMessage(message);  ///actually this is where we are getting segmentation fault
}

void Router::printRoutingTable(){
    cout<<"======================================"<<endl;
    cout<<"Routing table of "<<myIp<<":\nDestination :: Distance :: NextHop\n-----------------------------"<<endl;
    for(int i=0; i<allRouters.size(); i++){
        cout<<routingTable->get(allRouters[i])->toString()<<endl;
    }
    cout<<"======================================"<<endl;

}


void Router::sendRoutingTableToNeighbors(){
    string opcode = "rt";

    vector<unsigned char> bytes, tableData;

    pushBackStringToBytes(bytes, opcode);
    tableData = extractBytesFromTable();

    bytes.insert(bytes.end(), tableData.begin(), tableData.end());

    char buff[1024];
    for(int i=0; i<bytes.size(); i++){
        buff[i] = bytes[i];
    }

   for(int i=0; i<neighbors.size();i++){
        SendSocket* sock = sendSockets->get(neighbors[i]);

        if(sock!=null)
            sock->sendBytes(buff, bytes.size());
    }

}

vector<unsigned char> Router::extractBytesFromTable(){
    vector<unsigned char> bytes;
    int tableLength = allRouters.size();

    pushBackIntToBytes(bytes, tableLength);


    for(int i=0; i<tableLength; i++){

        string destinationIp = allRouters[i];
        RoutingInfo* info = routingTable->get(destinationIp);

        int distance = info->getDistance();
        string nextHopIp = info->getNextHop();

        pushBackIntToBytes(bytes, getUnsignedIp(destinationIp));  ///the 4 bytes is occupied by table Length---so
        pushBackIntToBytes(bytes, distance);
        pushBackIntToBytes(bytes, getUnsignedIp(nextHopIp));
    }
    return bytes;
}





int main(int argc, char** argv){

	if(argc<3){
		cout<<"Try executing with "<<argv[0]<<" <IP> <topo>\n";
		exit(1);
	}

	string myIp = argv[1];

    ifstream topo(argv[2]);

	vector<string> allRouters;
	vector< pair<string, int> > neighbors;

	set<string> routerSet; ///temporary placeHolder

	///populating with some real data-------------------------
	string ip1, ip2;
	int distance;

	while(topo){
        topo>>ip1>>ip2>>distance;
        if(ip1!=myIp) routerSet.insert(ip1);
        if(ip2!=myIp) routerSet.insert(ip2);

        if(ip1==myIp) neighbors.push_back(pair<string, int> (ip2, distance));
        else if(ip2==myIp) neighbors.push_back(pair<string, int> (ip1, distance));
       // cout<<ip1<<" "<<ip2<<" "<<distance<<endl;
    }
    topo.close();

    allRouters.insert(allRouters.begin(), routerSet.begin(), routerSet.end());

	///now time to start the router, it will do rest of the works--------------------------

	Router* router = new Router(myIp, DEFAULT_PORT, DRIVER_IP,allRouters,neighbors);

	router->start();

	return 0;

}
