#include <bits/stdc++.h>
#define INF 500

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
        if(distance>=INF){
            this->distance = INF;
            this->nextHop = "-";
        }
    }

    int getDistance(){return distance;}
    string getDestination() { return destination;}
    string getNextHop(){return nextHop;}

    string toString(){
        stringstream ss;
        ss<<destination<<" :: ";
        if(distance==INF) ss<<"INFINITY";
        else ss<<distance;
        ss<<" :: "<<nextHop;
        return ss.str();
    }

};


class NeighborInfo{
    string ip;
    int linkCost;
    int lastClock;
    bool down;
public:
    NeighborInfo(string ip, int linkCost, int lastClock = 0){
        down = false;

        this->ip = ip;
        this->linkCost = linkCost;
        this->lastClock = lastClock;
    }

    string getIp(){return ip;}
    int getCost(){return linkCost;}
    int getLastClock(){return lastClock;}

    void updateCost(int linkCost){ this->linkCost = linkCost;}
    void updateLastClock(int lastClock){this->lastClock = lastClock;}



    bool isDown(){return down;}
    void markDown(){down = true;}
    void markUp(){down = false;}
};
