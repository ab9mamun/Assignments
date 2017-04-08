#include <bits/stdc++.h>



using namespace std;

string getStringIp(unsigned u_address){
	stringstream ss;
	ss<<(u_address&0xff)<<"."<<((u_address&0xff00)>>8)<<"."<<((u_address&0xff0000)>>16)<<"."<<((u_address&0xff000000)>>24);
	return ss.str();
}
