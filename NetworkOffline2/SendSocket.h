#include <bits/stdc++.h>

using namespace std;

class SendSocket{

string receiverIp;
unsigned short port;

int sockfd;
struct sockaddr_in receiverAddress;

public:

    SendSocket(int sockfd, string receiverIp, unsigned short port){

        this->sockfd = sockfd;
        this->receiverIp = receiverIp;
        this->port = port;

        receiverAddress.sin_family = AF_INET;
        receiverAddress.sin_port = htons(port);
	//server_address.sin_addr.s_addr = inet_addr("192.168.10.100");
        inet_pton(AF_INET,receiverIp.c_str(),&receiverAddress.sin_addr);


    }


    void sendMessage(string message){

        int sent_bytes=sendto(sockfd, message.c_str(), 1024, 0, (struct sockaddr*) &receiverAddress, sizeof(sockaddr_in));

    }
    void sendBytes(vector<unsigned char> bytes){
        char buffer[1024];
        for(int i=0; i<bytes.size(); i++){
            buffer[i]=bytes[i];
        }
        int sent_bytes=sendto(sockfd, buffer, 1024, 0, (struct sockaddr*) &receiverAddress, sizeof(sockaddr_in));
    }



    int getSockfd(){return sockfd;}
    string getReceiverIp(){return receiverIp;}

};
