#include "viewer_iface.h"
#include "level.h"
#include "config.h"
#include "state.h"
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;
void renderDashboard(int playerFloor, int playerRoom,
                     int energy, int time, int score,
                     const char inventory[MAX_INVENTORY_CAPACITY],
                     int inventoryCount, int emergencyTripsLeft){
    cout<<"\n";
    cout<<setfill('=')<<setw(14)<<""<<" PLAYER DASHBOARD "<<setfill('=')<<setw(14)<<""<<endl;
    cout<<"Location          : Floor "<<playerFloor
        <<" | Room "<<playerRoom<<endl;
    cout<<"Score             : "<<score<<endl;
    cout<<"Energy            : "<<energy<<"/"<<MAX_ENERGY<<endl;
    cout<<"Remaining Time    : "<<time<<"/"<<MAX_TIME<<endl;
    cout<<"Emergency Trips   : "<<emergencyTripsLeft<<endl;
    cout<<"Inventory         : ";
    if (inventoryCount==0){
        cout<<"Empty";
    }
    else{
      for(int i=0;i<inventoryCount;i++){
        cout<<getEntityName(inventory[i]);
        if (i<inventoryCount-1){
            cout<<", ";
        }
        }
    }
    cout<<endl;
    cout <<setfill('=')<<setw(40)<<""<<endl;
}

