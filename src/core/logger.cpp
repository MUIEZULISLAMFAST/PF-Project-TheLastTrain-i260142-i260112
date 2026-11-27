#include "logger.h"
#include "util.h"
#include <fstream>
#include<iomanip>
//we use this because we cannot using fstream withfile namespace
using namespace std;
// File persistence for savegame.txt and game_summary.txt using primitive variables

bool saveGameStateToFile(const char* filename,
                         int playerFloor, int playerRoom,
                         int energy, int time, int score,
                         const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount,
                         const bool objectives[NUM_OBJECTIVES], int emergencyTripsLeft,
                         const char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                         const int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR]){
//as per always validation comes first
if(filename==nullptr || playerFloor<1 || playerFloor>NUM_FLOORS ||playerRoom<1 || playerRoom>ROOMS_PER_FLOOR || energy<0 || energy>MAX_ENERGY ||time<0 || time>MAX_TIME ||score<0 || score>MAX_SCORE ||inventory==nullptr ||inventoryCount<0 ||inventoryCount>MAX_INVENTORY_CAPACITY ||objectives==nullptr ||emergencyTripsLeft<0 || emergencyTripsLeft>2 ||roomEntities==nullptr || entityCounts==nullptr){
    return false;
}
//validating the no of entities in the entity room
for(int i=0;i<NUM_FLOORS;i++){
    for(int j=0;j<ROOMS_PER_FLOOR;j++){
        if(entityCounts[i][j]>MAX_ENTITIES_PER_ROOM || entityCounts[i][j]<0){
            return false;
        }
    }
}
//now opening file 
ofstream file(filename);
//checking if file is not openening
if(!file.is_open()){
    return false;
}
// now we are going to write title to our file
file<<setfill('=')<<setw(40)<<""<<endl;
file<<setfill(' ')<<setw(10)<<" LAST TRAIN : RECOVERY LOG "<<setfill(' ')<<setw(10)<<""<<endl;
file<<setfill('=')<<setw(40)<<""<<endl;
//writing information abfile player 
file<<"---Operator Location---"<<endl;
file<<"Floor: "<<playerFloor<<endl;
file<<"Room: "<<playerRoom<<endl;
//writing abfile survival metrices 
file<<"\n---Survival Metrices---"<<endl;
file<<"Energy: "<<energy<<endl;
file<<"Time: "<<time<<endl;
file<<"Score: "<<score<<endl;
//writing already equipped items 
file<<"\n---Equipment Manifest---"<<endl;
file<<"Inventory:";
for(int i=0;i<inventoryCount;i++){
    file<<" "<<inventory[i];
}
//now writing mission checklist that player has completed or not completed what objectives 
file<<"\n---Mission CheckPoint---"<<endl;
file<<"Objectives";
for(int i=0;i<NUM_OBJECTIVES;i++){
    file<<" "<<objectives[i];
}
//now storing faculty status
file<<"\n---Faculty Status---"<<endl;
file<<"EmergencyTrips: "<<emergencyTripsLeft<<endl;
//now storing entity counts in building state
file<<"---Building State---"<<endl;
file<<"Entity Counts:";
for(int i=0;i<NUM_FLOORS;i++){
    for(int j=0;j<ROOMS_PER_FLOOR;j++){
        file<<entityCounts[i][j]<<" ";
    }
    //new line
    file<<"\n";
}
//saving all rooms entities 
file<<"Room Entities:";
for(int i=0;i<NUM_FLOORS;i++){
    for(int j=0;j<ROOMS_PER_FLOOR;j++){
        for(int k=0;k<MAX_ENTITIES_PER_ROOM;k++){
            file<<" "<<roomEntities[i][j][k];
        }
        file<<"\n";
    }
}
//now displaying the end of recovery log
file<<"\n";
file<<setfill('=')<<setw(40)<<""<<endl;
file<<setfill(' ')<<setw(10)<<" End Of Recovery Log "<<setfill(' ')<<setw(10)<<""<<endl;
file<<setfill('=')<<setw(40)<<""<<endl;
//now closing file 
file.close();
//checking weather saving failed or not 
return !file.fail();
}