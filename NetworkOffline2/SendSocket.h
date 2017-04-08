#include <bits/stdc++.h>

using namespace std;

class SendSocket{

string receiverIp;
unsigned short port;

int sockfd;
struct sockaddr_in server_address;

public:

    SendSocket(int sockfd, string receiverIp, unsigned short port){

        this->sockfd = sockfd;
        this->receiverIp = receiverIp;
        this->port = port;

        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(port);
	//server_address.sin_addr.s_addr = inet_addr("192.168.10.100");
        inet_pton(AF_INET,receiverIp.c_str(),&server_address.sin_addr);


    }


    void sendMessage(string message){

        int sent_bytes=sendto(sockfd, message.c_str(), 1024, 0, (struct sockaddr*) &server_address, sizeof(sockaddr_in));

    }



    int getSockfd(){return sockfd;}

};
