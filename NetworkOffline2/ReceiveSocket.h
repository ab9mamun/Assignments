#include <bits/stdc++.h>

#include "util.h"

using namespace std;

class ReceiveSocket{


    string ip;
    unsigned short port;

    struct sockaddr_in myAddress;

    int sockfd;
    socklen_t addrlen;

    char buffer[1024];




    string getStringIp(unsigned u_address){
	stringstream ss;
	ss<<(u_address&0xff)<<"."<<((u_address&0xff00)>>8)<<"."<<((u_address&0xff0000)>>16)<<"."<<((u_address&0xff000000)>>24);
	return ss.str();
}

public:
	ReceiveSocket(string ip, unsigned short port) {
        this->ip = ip;
        this->port = port;

        myAddress.sin_family = AF_INET;
        myAddress.sin_port = htons(port);
        inet_pton(AF_INET,ip.c_str(), &myAddress.sin_addr);
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        int bind_flag = bind(sockfd, (struct sockaddr*) &myAddress, sizeof(sockaddr_in));
        if(bind_flag==0) cout<<"Successful bind\n";
	}

	Packet receivePacket(){
        struct sockaddr_in yourAddress;
        int bytes_received = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) &yourAddress, &addrlen);
        unsigned int u_address = yourAddress.sin_addr.s_addr;
        string sender = getStringIp(u_address);

        Packet pack(sender, buffer);
        return pack;
	}


	string getIp(){return ip;}
	unsigned short getPort(){return port;}
	int getSockfd(){return sockfd;}
};

