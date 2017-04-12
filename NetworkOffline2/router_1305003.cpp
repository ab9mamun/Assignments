#include <bits/stdc++.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

#include <string>
#include "RoutingInfo_1305003.h"
#include "ReceiveSocket_1305003.h"
#include "SendSocket_1305003.h"
#include "HashTable_1305003.h"

//let's define some macros
#define DRIVER_IP "192.168.10.100"
#define DEFAULT_PORT 4747




using namespace std;

///----------------------formalities aka prototypes--------------------


//-----
///---------------------global variables----------------------------------



class Router{

    int clk;

    ReceiveSocket* receiveSocket;
    StringHashTable<SendSocket>* sendSockets;
    StringHashTable<RoutingInfo>* routingTable;
    StringHashTable<NeighborInfo>* neighborTable;

    string myIp;
    unsigned short myPort;
    int sockfd;


    string driverIp;
    vector<string> neighbors;
    vector<string> allRouters;



public:

    Router(string myIp,unsigned short myPort, string driverIp, vector<string> allRouters, vector<pair<string, int> > neighborData){

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
        neighborTable = new StringHashTable<NeighborInfo>(17);  ///this will be needed for dvr and link up down----

        receiveSocket = new ReceiveSocket(myIp, myPort);
        sockfd = receiveSocket->getSockfd();

        for(int i=0; i<neighborData.size(); i++){
            string ip = neighborData[i].first;
            SendSocket* sock = new SendSocket(sockfd, ip, myPort);

           // cout<<ip<<" "<<sock<<endl;

            neighbors.push_back(ip);
            sendSockets->put(ip, sock);

            int distance = neighborData[i].second;
            NeighborInfo* ni = new NeighborInfo(ip, distance, 0);
            neighborTable->put(ip, ni);
            routingTable->get(ip)->update(distance, ip);  ///update in routing table for the neighbor...
        }

        printRoutingTable();

    }


    void start(){

       clk = 0;

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

    void dvrUpdate(string neighbor, string destination, int distance, string nextHop);

    void followDriverInstruction(Packet packet);
    void updateUsingNeighborsTable(string neighbor, vector<unsigned char> bytes, int whereToStart);
    void sendMessage(string ip, string message);
    void sendBytes(string ip, vector<unsigned char> bytes);
    void sendRoutingTableToNeighbors();
    void printRoutingTable();
    void followRouterInstruction(Packet packet);
    vector<unsigned char> extractBytesFromTable();
    void dvrNeighborUpdate(string neighbor, int newCost);

    void detectLinkReactivate(string neighbor);
    void detectLinkDeactivate(string neighbor);

    void neighborCostUpdate(string neighbor, int newCost);
    void sendPacket(string destination, string packetName, string message);
    void disconnectWhereNextHop(string neighbor);

    ~Router();


};






void Router::dvrUpdate(string neighbor, string destination, int distance, string nextHop){

    if(destination==myIp) return; ///life savior line---------forgetting this line cost 3hours ---- :-(

  //  cout<<neighbor<<" "<<destination<<" "<<distance<<" "<<nextHop<<endl;
   // return true;
    RoutingInfo* info = routingTable->get(destination);
    if(info==0) {cout<<"Bug found at dvrUpdate, RoutingInfo* is null"<<endl;}// return true;}
   // cout<<info->toString()<<endl;
   // return true;
    int myOldDistance = info->getDistance();
    string myOldNextHop = info->getNextHop();

   // return true;
    int myNewDistance = neighborTable->get(neighbor)->getCost() + distance;  ///d = cost(me, neighbor) + dist(neighbor, destination)

    if(myNewDistance>INF) myNewDistance = INF;

    if(myOldNextHop == neighbor ///***********important::: forced update
        || (myNewDistance < myOldDistance
            && myIp != nextHop) ///***********important::: split horizon
    ){

     info->update(myNewDistance, neighbor);


    }


}



void Router::dvrNeighborUpdate(string neighbor, int newCost){

    RoutingInfo* info = routingTable->get(neighbor);
    int distance = info->getDistance();
    string nextHop = info->getNextHop();

    if(nextHop==neighbor || newCost<distance){  ///forced update is must, but split horizon is not needed here-----------
        info->update(newCost, neighbor);

    }
}

void Router::neighborCostUpdate(string neighbor, int newCost){  ///returns true if routing path/distance is updated..

    NeighborInfo* ni = neighborTable->get(neighbor);
    if(ni==null){cout<<"can't update cost. the other router is not my neighbor\n"; return; }
    ni->updateCost(newCost);

    if(ni->isDown()==false)
    dvrNeighborUpdate(neighbor, newCost);


}

void Router::disconnectWhereNextHop(string neighbor){
    for(int i=0; i<allRouters.size(); i++){
        string ip = allRouters[i];
        RoutingInfo* info = routingTable->get(ip);
        if(info->getNextHop()==neighbor){
            info->update(INF, "-");
        }

    }
}


void Router::detectLinkDeactivate(string neighbor){

    NeighborInfo* ni = neighborTable->get(neighbor);
    if(ni==null){cout<<"bug at link deactivate"<<endl; return;}

    if(ni->isDown()) return; ///done from before;;;

    int lastClk = ni->getLastClock();
    if(clk-3 > lastClk){
        ni->markDown();
        //dvrNeighborUpdate(neighbor, INF); --this line is no more needed
        disconnectWhereNextHop(neighbor);
    }


}

void Router::detectLinkReactivate(string neighbor){

    NeighborInfo* ni = neighborTable->get(neighbor);
    if(ni==0){cout<<"bug at link reactivate"<<endl; return;}
    ni->updateLastClock(clk);
    if(ni->isDown()){
        ni->markUp();
        dvrNeighborUpdate(neighbor, ni->getCost());
    }

}

void Router::sendPacket(string destination, string packetName, string message){
    if(destination==myIp){
        cout<<packetName<<" packet reached destination.\n";
    }
    else {
        RoutingInfo* info = routingTable->get(destination);
        if(info==0){cout<<"bug found at sendPacket\n"; return;}
        string nextHop = info->getNextHop();
        if(nextHop=="-"){
            cout<<"Couldn't send "<<packetName<<" packet. No path known for "<<destination<<endl;
            return;
        }
        sendMessage(nextHop, message);
        cout<<packetName<<" packet forwarded to "<<nextHop<<endl;
    }
}


void Router::followDriverInstruction(Packet packet){

    string message = packet.getMessage();
    vector<unsigned char> bytes = packet.getBytes();
    //vector<string> params = split(message, ' ');
    if(startsWith(message,"send")){
        //cout<<"Driver says send"<<endl;
        string ip1 = extractIpFromBytes(bytes, 4);
        string ip2 = extractIpFromBytes(bytes, 8);
        unsigned short msgLength = (unsigned short) extractIntFromBytes(bytes, 12, 2);
        string msg = extractStringFromBytes(bytes, 14, msgLength);

        stringstream ss;
        ss<<"frwd#"<<ip2<<"#"<<msgLength<<"#"<<msg;

        sendPacket(ip2, msg, ss.str());

    }

    else if(startsWith(message, "show")){
     //   cout<<"Driver says show"<<endl;
        printRoutingTable();
    }

    else if(startsWith(message, "cost")){

      //  cout<<"Driver says cost"<<endl;
        string ip1 = extractIpFromBytes(bytes, 4);
        string ip2 = extractIpFromBytes(bytes, 8);
        int cost = (int) extractIntFromBytes(bytes, 12, 2);

        if(ip1==myIp)
            neighborCostUpdate(ip2, cost);
        else if(ip2==myIp)
            neighborCostUpdate(ip1, cost);

      //  cout<<ip1<<" "<<ip2<<" "<<cost<<endl;


    }
    else if(startsWith(message, "clk")){
        vector<string> v = split(message, ' ');
        int clk = atoi(v[1].c_str());
        this->clk = clk;

       // cout<<"Driver says clk "<<clk<<endl;
        sendRoutingTableToNeighbors();

        for(int i=0; i<neighbors.size(); i++){
          //  cout<<neighbors[i]<<endl;
            detectLinkDeactivate(neighbors[i]);

        }
    }

   // printRoutingTable();
//   sendMessage(allRouters[1], message);
}

void Router::followRouterInstruction(Packet packet){
    string message = packet.getMessage();
    string sender = packet.getSenderIp();

    if(routingTable->get(sender)==null){//cout<<"ignoring message from unknown source.."<<endl;
        return;
    }
    detectLinkReactivate(sender);

    if(startsWith(message, "rt")){
        updateUsingNeighborsTable(sender, packet.getBytes(), 2);
    }

    else if(startsWith(message, "frwd")){

        vector<string> v = split(message, '#');

        if(v.size()<4){cout<<"bug found at frwd from router\n"; return;}

        string ip = v[1];
        unsigned short msgLength = (unsigned short) atoi(v[2].c_str());
        string msg = "";

        for(int i=3; i<v.size(); i++){
            msg+= v[i];
        }

        stringstream ss;
        ss<<"frwd#"<<ip<<"#"<<msgLength<<"#"<<msg;

        sendPacket(ip, msg, ss.str());
    }

}



void Router::updateUsingNeighborsTable(string neighbor, vector<unsigned char> bytes, int whereToStart){

    int offset = whereToStart;

        int length = extractIntFromBytes(bytes, offset);
        offset+= 4;
       // cout<<length<<endl;
       // cout<<"Got routing table of "<<neighbor<<":\n";
        for(int i=0; i<length; i++){
            string destination = getStringIp(extractIntFromBytes(bytes, offset));
            int distance = extractIntFromBytes(bytes, offset+4);
            string nextHop = getStringIp(extractIntFromBytes(bytes, offset+8));

            offset+=12;

           // cout<<destination<<" :: "<<distance<<" :: "<<nextHop<<endl;
          dvrUpdate(neighbor, destination, distance, nextHop);
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






Router:: ~Router(){
    if(routingTable){
      delete routingTable; ///RoutingInfo* will be deleted inside ~StringHashTable(), so, we dont need to worry about it
    }
    if(sendSockets){
        delete sendSockets; ///same for this one
    }
    if(receiveSocket){
        delete receiveSocket;
    }
    if(neighborTable){
        delete neighborTable;
    }
}



int main(int argc, char** argv){

	if(argc<3){
		cout<<"Try executing with "<<argv[0]<<" <IP> <topo>\n";
		exit(1);
	}

	string myIp="";
	myIp += argv[1];

    ifstream topo(argv[2]);

	vector<string> allRouters;
	vector< pair<string, int> > neighbors;

	set<string> routerSet; ///temporary placeHolder

	///populating with some real data-------------------------
	string ip1, ip2;
	int distance;

	 string line;

    if (topo.is_open()) {
        while ( getline (topo,line) ){

          vector<string> v = split(line, ' ');
            if(v.size()!=3){ cout<<"faulty file. "<<endl; exit(1);}

          ip1 = v[0];
          ip2 = v[1];
          distance = atoi(v[2].c_str());


        if(ip1!=myIp) routerSet.insert(ip1);
        if(ip2!=myIp) routerSet.insert(ip2);

        if(ip1==myIp) neighbors.push_back(pair<string, int> (ip2, distance));
        else if(ip2==myIp) neighbors.push_back(pair<string, int> (ip1, distance));

        }
        topo.close();
    }
    else {cout << "Unable to open file\n"; exit(1);}


    allRouters.insert(allRouters.begin(), routerSet.begin(), routerSet.end());

  //  for(int i=0; i<neighbors.size(); i++){
       // cout<<neighbors[i].first<<" "<<neighbors[i].second<<endl;
  //  }

	///now time to start the router, it will do rest of the works--------------------------

	Router* router = new Router(myIp, DEFAULT_PORT, DRIVER_IP,allRouters,neighbors);

	router->start();

	return 0;

}
