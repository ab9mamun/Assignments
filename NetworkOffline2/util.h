#include <bits/stdc++.h>

using namespace std;

typedef union {
        unsigned number;
        unsigned char bytes[4];
 } IP;


class Packet{
	string senderIp;
	string message;
    vector<unsigned char> bytes;

public:
	Packet(string sender, char buff[], int bytesRead){

        for(int i=0; i<bytesRead; i++){
            bytes.push_back((unsigned char) buff[i]);
        }
		senderIp = sender;
		message = "";
		message += buff;
	}
	string getMessage(){
		return message;
	}
	string getSenderIp(){
		return senderIp;
	}
    vector<unsigned char> getBytes(){return bytes;}
};







vector<string> split(string buff, char delim, int startFrom = 0){
    vector<string> v;

    buff+= delim; ///this is the life saver

    string temp="";
   // cout<<"splitting "+buff<<endl;
    for(int i=startFrom; i<buff.length(); i++){
      //  printf("%d ",buff[i]);
        if(buff[i]==delim){
            if(temp.length()==0) continue;
            v.push_back(temp);
            temp = "";
        }
        else temp+= buff[i];
    }
   // if(temp.length!=0)
    return v;
}

vector<string> split(string buff, char delim, int startFrom, int length){
    vector<string> v;

    buff+= delim; ///this is the life saver

    string temp="";
  //  cout<<"splitting "+buff<<endl;
    for(int i=startFrom; i<buff.length() && i<(startFrom+length); i++){
      //  printf("%d ",buff[i]);
        if(buff[i]==delim){
            if(temp.length()==0) continue;
            v.push_back(temp);
            temp = "";
        }
        else temp+= buff[i];
    }
   // if(temp.length!=0)
    return v;
}

string extractIpFromBytes(vector<unsigned char> buff, int start){
    stringstream ss;
    for(int i=0; i<3; i++){
        ss<< (unsigned int) buff[start+i]<<".";
    }
    ss<< (unsigned int)buff[start+3];
    return ss.str();
}

int extractIntFromBytes(vector<unsigned char> buff, int start, int length = 4){
    int x = 0;
    unsigned char byte;
    for(int i=0; i<length; i++){
        byte = buff[start+i];
      //  cout<<byte<<endl;
        x|= (((unsigned int) byte)<<(i*8));
    }
    return x;
}

string extractStringFromBytes(vector<unsigned char> buff, int start, int length){
    string s = "";
    for(int i=start; i<(start+length); i++){
       // printf("%d ", buff[i]);
        s+= (char) buff[i];
    }
    return s;
}


bool startsWith(string buff, string sub){
    if(sub.length()>buff.length()) return false;
    return buff.substr(0, sub.length())==sub;
}























string getStringIp(unsigned u_address){
	stringstream ss;
	IP ip;
	ip.number = u_address;

	for(int i=0; i<3; i++){
        ss<<(unsigned int) ip.bytes[i] <<".";
	}
	ss<<(unsigned int) ip.bytes[3];

	return ss.str();

}

unsigned int getUnsignedIp(string ip){

    vector<string> v = split(ip, '.');
    if(v.size()!=4) {cout<<"Bug at getUnsignedIp, "; return 0;}

    IP newIp;
    for(int i=0; i<4; i++){
        newIp.bytes[i] = (unsigned char) atoi(v[i].c_str());
    }
    return newIp.number;
}

void pushBackIntToBytes(vector<unsigned char>& v, int x){
    IP ip;
    ip.number = (unsigned int) x;
    for(int i=0; i<4; i++){
        v.push_back(ip.bytes[i]);
    }

}

void pushBackStringToBytes(vector<unsigned char>& v,string s){
    for(int i=0; i<s.length(); i++){
        v.push_back(s[i]);
    }
}


void setIntInBytes(unsigned char* buff, int x, int startFrom){
    IP ip;
    ip.number = (unsigned int) x;
    for(int i=0; i<4; i++){
        buff[startFrom+i] = ip.bytes[i];
    }
}
