#include <bits/stdc++.h>

using namespace std;

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


string getStringIp(unsigned u_address){
	stringstream ss;
	ss<<(u_address&0xff)<<"."<<((u_address&0xff00)>>8)<<"."<<((u_address&0xff0000)>>16)<<"."<<((u_address&0xff000000)>>24);
	return ss.str();

}
vector<string> split(string buff, char delim, int startFrom = 0){
    vector<string> v;

    buff+= delim; ///this is the life saver

    string temp="";
    cout<<"splitting "+buff<<endl;
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
    cout<<"splitting "+buff<<endl;
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
