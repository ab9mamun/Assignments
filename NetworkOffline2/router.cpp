#include <bits/stdc++.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

#include <string>
#include "RoutingTable.h"
#include "util.h"

using namespace std;

///----------------------formalities aka prototypes--------------------
string getStringIp(unsigned int);

//-----



int main(){

	string message;
	unsigned short port;
	string ip;
	unsigned u_address;

	int sockfd; 
	int bind_flag;
	int bytes_received;
	socklen_t addrlen;
	char buffer[1024];
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;




	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(4747);
	inet_pton(AF_INET,"192.168.10.1", &server_address.sin_addr);
	
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	
	bind_flag = bind(sockfd, (struct sockaddr*) &server_address, sizeof(sockaddr_in));
	
	if(bind_flag==0)printf("successful bind\n");

	while(true){
		bytes_received = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) &client_address, &addrlen);
		
		message = "";
		message += buffer;
		
		u_address = client_address.sin_addr.s_addr;

		ip = getStringIp(u_address);

		port = ntohs(client_address.sin_port);
		
		
		//printf("[%s:%hu]: %s\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port), buffer);
		
		//printf("%hu\n",client_address.sin_port);
		//printf("%u\n",client_address.sin_addr.s_addr);
		
		cout<<ip<<" "<<port<<" "<<message<<endl;
		//cout<<<<endl;

		if(strcmp(buffer,"shutdown")==0)break;		

	}

	return 0;

}
