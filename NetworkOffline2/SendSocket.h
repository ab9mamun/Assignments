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

        int sent_bytes=sendto(sockfd, message.c_str(), message.length(), 0, (struct sockaddr*) &receiverAddress, sizeof(sockaddr_in));

    }
    void sendBytes(vector<unsigned char> bytes){
        char buffer[1024];

        for(int i=0; i<bytes.size(); i++){
            buffer[i]= (char) bytes[i];
        }
     //   cout<<buffer[0]<<buffer[1]<<endl;
        //cout<<bytes.size()<<endl;
        /*for(int i=2; i<bytes.size(); i+=4){
            IP x;
            x.bytes[0] = buffer[i];
            x.bytes[1] = buffer[i+1];
            x.bytes[2] = buffer[i+2];
            x.bytes[3] = buffer[i+3];
            cout<<x.number<<" ";
        }*/
        int sent_bytes=sendto(sockfd, buffer, bytes.size(), 0, (struct sockaddr*) &receiverAddress, sizeof(sockaddr_in));
        //cout<<sent_bytes<<endl;
    }



    int getSockfd(){return sockfd;}
    string getReceiverIp(){return receiverIp;}

};
