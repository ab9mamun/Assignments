#include <bits/stdc++.h>
#define INF 50

using namespace std;

class RoutingInfo {
    string destination;
    int distance;
    string nextHop;

public:

    RoutingInfo(string destination){
        this->destination = destination;
        distance = INF;
        nextHop = "-";
    }
    void update(int distance, string nextHop){
        this->distance = distance;
        this->nextHop = nextHop;
    }

    int getDistance(){return distance;}
    string getDestination() { return destination;}
    string getNextHop(){return nextHop;}

    string toString(){
        stringstream ss;
        ss<<destination<<" :: "<<distance<<" :: "<<nextHop;
        return ss.str();
    }

};
