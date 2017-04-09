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




vector<string> split(string buff, char delim){
    vector<string> v;
    string temp="";
    for(int i=0; i<buff.length(); i++){
        if(buff[i]==delim){
            if(temp.length()==0) continue;
            v.push_back(temp);
            temp = "";
        }
        else temp+= buff[i];
    }
    return v;
}
