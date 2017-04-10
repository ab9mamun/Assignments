#include <bits/stdc++.h>
#include "util.h"
using namespace std;

int main(){


    //cout<<getStringIp(34252992);
    vector<pair<string, int> > neighbors;
    set<string> routerSet;
    vector<string> allRouters;
    string myIp = "192.168.10.1";

    ifstream topo("topo.txt");
    string ip1, ip2;
    int distance;


    while(topo){
        topo>>ip1>>ip2>>distance;
        if(ip1!=myIp) routerSet.insert(ip1);
        if(ip2!=myIp) routerSet.insert(ip2);

        if(ip1==myIp) neighbors.push_back(pair<string, int> (ip2, distance));
        else if(ip2==myIp) neighbors.push_back(pair<string, int> (ip1, distance));
       // cout<<ip1<<" "<<ip2<<" "<<distance<<endl;
    }
    topo.close();

    allRouters.insert(allRouters.begin(), routerSet.begin(), routerSet.end());

    cout<<"All Routers:\n";
    for(int i=0; i<allRouters.size(); i++){
        cout<<allRouters[i]<<endl;
    }

    cout<<"All neighbors:\n";
    for(int i=0; i<neighbors.size(); i++){
        cout<<neighbors[i].first<<" "<<neighbors[i].second<<endl;
    }

}
