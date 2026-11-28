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

void renderRoomDetails(int currentFloor, int currentRoom,
                       const char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                       int totalEntities){
    cout<<"\n";
    cout<<setw(42)<<"ROOM SCANNER"<<endl;
    cout<<setfill('=')<<setw(42)<<""<<endl;
    //valiadting the parameters of function as per our decison
    if (currentFloor<1 || currentFloor>NUM_FLOORS || currentRoom<1 || currentRoom> ROOMS_PER_FLOOR){
        cout<<"Invalid room location!"<<endl;
        return;
    }
    //valiadting entities count 
    if (totalEntities<0 || totalEntities>MAX_ENTITIES_PER_ROOM){
        cout<<"Invalid room data!"<<endl;
        return;
    }
    cout<<left<<setw(19)<<"Location"<< ": Floor "<<currentFloor<<" - Room "<<currentRoom<<endl;
    cout<<left<<setw(19)<<"Objects Found"<<": "<<totalEntities<<endl;
    cout<<left<<setw(19)<<"Room Status"<< ": ";
    if (totalEntities==0){
        cout<<"CLEAR"<<endl;
    }
    else if (totalEntities<=2){
        cout<<"LOW ACTIVITY"<<endl;
    }
    else{
        cout<<"HIGH ACTIVITY"<<endl;
    }
    cout<<setfill('-')<<setw(42)<<""<<setfill(' ')<<endl;
    if (totalEntities==0){
        cout<<"This room is currently empty."<<endl;
    }
    else{
        cout<<left<<setw(10)<<"SLOT"<<setw(28)<<"ENTITY"<<endl;
        cout<<setfill('-')<<setw(42)<<""<<setfill(' ')<<endl;
        for (int k=0;k<totalEntities;k++){
            char currentSymbol=roomEntities[currentFloor-1][currentRoom-1][k];
            cout<<left<<setw(10)<<k+1<<setw(28)<<getEntityName(currentSymbol)<<endl;
        }
    }
    cout<<setfill('=')<<setw(42)<<""<<setfill(' ')<<endl;
}
void renderAsciiBuilding(int playerFloor,int playerRoom){
    // Checking functions parameters
    if (playerFloor<1 || playerFloor>NUM_FLOORS || playerRoom<1 || playerRoom> ROOMS_PER_FLOOR){
        cout<<"Invalid player location!"<<endl;
        return;
    }
    ifstream file("data/building.txt");
    if(!file){
        cout<<"Unable to open building.txt!"<<endl;
        return;
    }
    char line[MAX_LINE_LEN];
    char entityLine[MAX_LINE_LEN];
    cout<<endl;
    while(file.getline(line,MAX_LINE_LEN)){
        cout<<line<<endl;
        // now identifying the entity lines so that we can make entities not visible to player beacuse he cannot be able to enties in each room
        bool roomLine=false;
        for (int i=0; line[i]!='\0';i++){
            if (line[i]=='F' && line[i+1]>='1' && line[i+1]<='5' && line[i+2]=='-'&& line[i+3]=='R'){
                roomLine=true;
                break;
            }
        }
        //reading entities lines below the room line 
        if (roomLine && file.getline(entityLine, MAX_LINE_LEN)){
           //now making entites hide from approach of player
           for (int i = 0; entityLine[i] != '\0'; i++){
            if(entityLine[i]=='<'||entityLine[i]==')' ||entityLine[i]=='\\' || entityLine[i]=='4' || entityLine[i]=='*' || entityLine[i]=='~' || entityLine[i] == ';' || entityLine[i] == '(' ||entityLine[i] == '!' || entityLine[i] == '%' ||entityLine[i] == '$' || entityLine[i] == '2' ||
            entityLine[i] == '7' || entityLine[i] == '^' || entityLine[i] == '@' || entityLine[i] == '&')
            {
                //replacing entity with empty space
                entityLine[i]=' ';
            }
            }
            //now after loading the map showing player its position
            for (int i=0;line[i]!='\0';i++){
                if (line[i]=='F'&&
                    line[i+1]=='0'+playerFloor && line[i+2]=='-' && line[i+3]=='R' &&
                    line[i+4]=='0'+playerRoom){
                    entityLine[i+2]='P';
                    break;
                }
            }
            //after making changes to entity line we are printing it 
            cout<<entityLine<<endl;
        }
    }
    file.close();
    cout<<"\n";
    cout<<"P =Current Position Of Player"<<endl;
    cout<<"Room entities are hidden until inspection."<<endl;
}
void renderObjectivesChecklist(const bool objectives[NUM_OBJECTIVES]){
    const char* objectiveNames[NUM_OBJECTIVES]={
        "Master Key", "Clue 4", "Clue 2", "Clue 7","Cabinet Unlocked", "Train Key", "Fuse Collected","Fuse Installed", "Lift Pass", "Lift 2 Activated","Passenger Rescued", "Cat Carrier", "Cat Secured","Train Prepared", "Mission Won", "Mission Failed"
    };
    int completed=0;
    int total=NUM_OBJECTIVES-2;
    cout<<endl;
    cout<<setfill('=')<<setw(42)<<""<<setfill(' ')<<endl;
    cout<< "              MISSION CHECKLIST"<<endl;
    cout<<setfill('=')<<setw(42)<<""<<setfill(' ')<<endl;
    cout <<left<<setw(5)<<"NO."<<setw(12) << "STATUS" <<"OBJECTIVE"<<endl;
    cout << setfill('-')<<setw(42)<< "" <<setfill(' ')<<endl;
    for (int i=0; i<total;i++) {
        cout<<left<<setw(5)<<i+1;
        if (objectives[i]){
            cout<<setw(12)<<"[DONE]";
            completed++;
        }
        else{
            cout<<setw(12)<<"[PENDING]";
        }
        cout<<objectiveNames[i]<<endl;
    }
    cout<<setfill('-')<<setw(42)<<""<<setfill(' ')<<endl;
    cout<<"Completed: "<<completed<<" / "<< total<<endl;
    cout<<"Remaining: "<<total-completed<<endl;
    cout<<"\nProgress: [";
    for (int i=0;i<total;i++) {
        if (i<completed)
            cout<<"#";
        else{
            cout<<"-";
        }
    }
    cout<<"]"<<endl;
    cout<<setfill('=')<<setw(42)<<""<<setfill(' ')<<endl;
    if (objectives[OBJ_MISSION_WON]){
        cout<<"Mission completed successfully!"<<endl;
    }
    else if(objectives[OBJ_MISSION_FAILED]){
            cout<<"Mission failed!"<<endl;
    }
    else{
        cout<<"Complete the remaining objectives!" << endl;
    }
    cout<<setfill('=')<<setw(42)<<""<<setfill(' ')<<endl;
}
