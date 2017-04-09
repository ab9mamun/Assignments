#include <bits/stdc++.h>

#include "util.h"

using namespace std;

class ReceiveSocket{


    string ip;
    unsigned short port;

    struct sockaddr_in myAddress;

    int sockfd;
    socklen_t addrlen;

    //char buffer[1024];

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
        char buffer[1024];
        int bytes_received = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) &yourAddress, &addrlen);

        buffer[bytes_received] = 0;

        unsigned int u_address = yourAddress.sin_addr.s_addr;
        string sender = getStringIp(u_address);

        Packet pack(sender, buffer,bytes_received);
        return pack;
	}


	string getIp(){return ip;}
	unsigned short getPort(){return port;}
	int getSockfd(){return sockfd;}
};

