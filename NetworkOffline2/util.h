#include <bits/stdc++.h>



using namespace std;

class Packet{
	string senderIp;
	string message;

public:
	Packet(string sender, string msg){
		senderIp = sender;
		message = msg;
	}
	string getMessage(){
		return message;
	}
	string getSenderIp(){
		return senderIp;
	}
};

