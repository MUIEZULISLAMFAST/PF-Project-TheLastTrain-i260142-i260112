#include<iostream>
#include "config.h"
#include "simulator.h"
#include "physics.h"
#include "switches.h"
#include "logger.h"
#include<iomanip>
#include "viewer_iface.h"
#include "util.h"
#include "level.h"
#include <iomanip>
using namespace std;
// Modular simulation loop using primitive variables
//function 1-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void runSimulation(int& playerFloor, int& playerRoom,
                   int& energy, int& time, int& score,
                   char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                   bool objectives[NUM_OBJECTIVES], int& emergencyTripsLeft,
                   char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1],
                   char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                   int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR]){
//first checking all validations 
//1.checking arrays address are valid or not 
if(inventory==nullptr || objectives==nullptr || horizontalConn==nullptr || roomEntities==nullptr || entityCounts==nullptr){
    cout<<"Invalid Game Data";
    return ;
}
//checking validity of all variables  
if (playerFloor>NUM_FLOORS || playerFloor<1 || playerRoom>ROOMS_PER_FLOOR || playerRoom<1 || energy>MAX_ENERGY || energy<0 || time<0 || time>MAX_TIME || inventoryCount>MAX_INVENTORY_CAPACITY || inventoryCount<0 || emergencyTripsLeft >3 || emergencyTripsLeft<0 || score<0 || score>MAX_SCORE){
  cout<<"Invalid Initial State";
  return;
}
//validating entity counts 
for(int i=0; i<NUM_FLOORS;i++){
    for(int j=0;j<ROOMS_PER_FLOOR;j++){
        if(entityCounts[i][j]>MAX_ENTITIES_PER_ROOM || entityCounts[i][j]<0){
            cout<<"invalid entity count";
            return;
        }
    }
}
//validating starting room and floor
if(!validateStartingRoom(playerFloor,playerRoom,horizontalConn)){
    cout<<"invalid starting room";
    return;
}
//after all validation now runn the game
bool runningStatus=true;
//we want it beacuse we want player to perform multiple actions e.g move, inspect,collect item etc etc 
while(runningStatus){
    //this function checks weather game has ended or not 
    //complte misiion 
    //run out of energy
    //run out of time 
    if(checkEndConditions(energy,time,objectives)){
        //if game not won and resourses are finished then 
        //we implement it here beacuse we cannot implement it in the checkendconditions function because constant array is there. 
        if(!objectives[OBJ_MISSION_WON] && (energy<=0 || time<=0)){
            objectives[OBJ_MISSION_FAILED]=true;
        }
        runningStatus=false;
        break;
    }
    //function for printing header------
    printHeader("THE LAST TRAIN ");
    //this functions job is to display current status of player e.g floor and room , energy etc etc
    renderDashboard(playerFloor,playerRoom,energy,time,score,inventory,inventoryCount,emergencyTripsLeft);
    //this function displays the original ascii bulding map 
    renderAsciiBuilding(playerFloor,playerRoom);
    //this display information regarding current room
    //roomentities complete 3D array
    //entitycounts this select entity count for players current room

    renderRoomDetails(playerFloor,playerRoom,roomEntities,entityCounts[playerFloor-1][playerRoom-1]);
    //displaying the menu to player in game 
    cout<<"============GAME CONTROLS==========="<<endl;
    cout<<"\n1. Move to another room"<<endl;
    cout<<"2. Inspect current room"<<endl;
    cout<<"3. Collect item"<<endl;
    cout<<"4. Use an inventory item"<<endl;
    cout<<"5. Unlock cabniet"<<endl;
    cout<<"6. View Objectives"<<endl;
    cout<<"7. Save game"<<endl;
    cout<<"8. Quit game"<<endl;
    cout<<"================================="<<endl;
    cout<<"Enter command : ";
    int choice;
    if (!(cin>>choice)){
        //clear the cin faliure 
        cin.clear();
        //clear rthe buffer value of cin
        cin.ignore(1000,'\n');
        cout<<"\nInvalid input. Enter a number from 1 to 8."<<endl;
        //put loop directly to next iteration 
        continue;
    }
    // using switch for menu 
    switch(choice){
    case 1:{
    int targetFloor,targetRoom;
    cout<<"\nEnter destination floor : ";
    if(!(cin>>targetFloor)){
        cin.clear();
        cin.ignore(1000,'\n');
        cout<<"Invalid floor input"<<endl;
        break;
    }
    cout<<"Enter destination room : ";
    if(!(cin>>targetRoom)){
        cin.clear();
        cin.ignore(1000,'\n');
        cout<<"Invalid room input."<<endl;
        break;
    }
    //validation of data took from user recently
    if(targetFloor<1 || targetFloor>NUM_FLOORS || targetRoom<1|| targetRoom>ROOMS_PER_FLOOR){
        cout<<"inalid destinantion."<<endl;
        break;
    }
    // function check weather movement is actually possible or not if allowed it updates players location and resources
    handleMoveCommand(targetFloor,targetRoom,playerFloor,playerRoom,energy,time,horizontalConn,inventory,inventoryCount,emergencyTripsLeft,objectives[9]);
    break;}
    //objective[9] is liftoperational20
    case 2:
    //the inspection handeler process inspection , resource costs , and anyother applicable rewars
    handleInspectCommand(playerFloor,playerRoom,energy,time,roomEntities,entityCounts[playerFloor-1][playerRoom-1],objectives,score);
    break;
    case 3 :{
    //first check no of entities in current room 
    int currentCount=entityCounts[playerFloor-1][playerRoom-1];
    //if no entity then breaks with suitable statment
    if(currentCount<=0){
        cout<<"\n There are no entities to collect here. "<<endl;
        break;
    }
    //displaying the entities present in this room 
    cout<<"\n=============Avalible Items============="<<endl;
    for(int i=0;i<currentCount;i++){
        cout<<i+1<<". "<<roomEntities[playerFloor-1][playerRoom-1][i]<<endl;
    }
    //asking user to select what he wants 
    cout<<"Select item no : ";
    int selected;
    //vallidating choice
    if(!(cin>>selected)){
        cin.clear();
        cin.ignore(1000,'\n');
        cout<<"Invalid selection"<<endl;
        break;
    }
    //vallidating choice
    if(selected<1 || selected>currentCount){
        cout<<"Invalid item number."<<endl;
        break;
    }
    //checking which caracter player selected
    char selectedItem=roomEntities[playerFloor-1][playerRoom-1][selected-1];
    //this function will deal with logic later like removal of entiy if user selected it and multiple more rules 
    handleCollectCommand(selectedItem,playerFloor,playerRoom,energy,time,score,roomEntities,entityCounts[playerFloor-1][playerRoom-1],inventory,inventoryCount,objectives);
    break; }
    case 4: {
    //validation
    if (inventoryCount<=0){
        cout<<"\nYour Inventory is Empty"<<endl;
        break;
    }
    //display items 
    cout<<"\n=================Your Inventory================="<<endl;
    //displaying players inventory
    for(int i=0;i<inventoryCount;i++){
        cout<<i+1<<". "<<inventory[i]<<endl;
    }
    //let player select item from his inventory
    cout<<"Select Item number to use: ";
    int selected;
    if(!(cin>>selected)){
        cin.clear();
        cin.ignore(1000,'\n');
        cout<<"Invalid Selection. "<<endl;
        break;
    }
    //validation of selected inventory item 
    if(selected<1||selected>inventoryCount){
        cout<<"Invalid inventory selection. "<<endl;
        break;
    }
    //selected item storage
    char selectedItem=inventory[selected-1];
    //this handeler will decide weather this selected item can be used in current situation  or not e.g cat carrier cannot unlock door 
    handleUseCommand(selectedItem,playerFloor,playerRoom,energy,time,score,inventory,inventoryCount,objectives);
    break;}
    case 5:{
    //we created a array to store entered code
    char enterCode[MAX_LINE_LEN];
    cout<<"\nEnter canbniet Code";
    if(!(cin>>setw(MAX_LINE_LEN)>>enterCode)){
        cin.clear();
        cin.ignore(1000,'\n');
        cout<<"invalid code input."<<endl;
        break;
    }
    //this handler will check code,logic and other conditions
    handleUnlockCabinetCommand(enterCode,playerFloor,playerRoom,objectives,score,roomEntities,entityCounts[playerFloor-1][playerRoom-1]);
    break;
    }
    case 6: {
        //displaying objectives
        cout<<"\n============== Mission Objectives ============="<<endl;
        //this will display objectives checklist which objectives are completed and which are pending 
        renderObjectivesChecklist(objectives);
        break;
    }
    case 7: {
        //we provide address of file where we want  to store players progress
        bool save=saveGameStateToFile("out/savegame.txt",playerFloor,playerRoom,energy,time,score,inventory,inventoryCount,objectives,emergencyTripsLeft,roomEntities,entityCounts);
        //function return bolean that game is saved or not 
        if(save){
            cout<<"\nGame saved successfully"<<endl;
        }
        else{
            cout<<"\nUnable to save this game"<<endl;
        }
        break;
    }
    case 8:{
        //quit the game 
        cout<<"\nYou have quit the game."<<endl;
        runningStatus=false;
        break;
    }
    default:{
        cout<<"\nInvalid choice. Select an option from 1 to 8."<<endl;
        break;

    }
    }
    //check end conditions again 
    if(checkEndConditions(energy,time,objectives)){
        if(!objectives[OBJ_MISSION_WON] && (energy<=0 || time<=0)){
            objectives[OBJ_MISSION_FAILED]=true;
        }
    runningStatus=false;
}
}
//objective 14 corresponds to misiion won and objective [15] to mission fail
if(objectives[14]||objectives[15]){
    writeGameSummaryReport("out/game_summary.txt",score,energy,time,objectives,objectives[14]);
}
}































