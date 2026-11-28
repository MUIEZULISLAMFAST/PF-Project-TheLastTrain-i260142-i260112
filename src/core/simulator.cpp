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
if (playerFloor>NUM_FLOORS || playerFloor<1 || playerRoom>ROOMS_PER_FLOOR || playerRoom<1 || energy>MAX_ENERGY || energy<0 || time<0 || time>MAX_TIME || inventoryCount>MAX_INVENTORY_CAPACITY || inventoryCount<0 || emergencyTripsLeft >2 || emergencyTripsLeft<0 || score<0 || score>MAX_SCORE){
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
    handleMoveCommand(targetFloor,targetRoom,playerFloor,playerRoom,energy,time,horizontalConn,inventory,inventoryCount,emergencyTripsLeft,objectives[OBJ_LIFT2_ACTIVE]);
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
if(objectives[OBJ_MISSION_WON]||objectives[OBJ_MISSION_FAILED]){
    writeGameSummaryReport("out/game_summary.txt",score,energy,time,objectives,objectives[OBJ_MISSION_WON]);
}
}
//function 2--
bool handleMoveCommand(int targetFloor, int targetRoom,
                       int& currentFloor, int& currentRoom,
                       int& energy, int& time,
                       const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1],
                       const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount,
                       int& emergencyTripsLeft, bool lift2Operational){
//validation destinantions
if(targetFloor<1 || targetFloor>NUM_FLOORS || targetRoom<1 || targetRoom>ROOMS_PER_FLOOR){
    cout<<"Entered destination is invalid "<<endl;
    return false;
}
//checking weather player is not in same room or not 
if(targetFloor==currentFloor && targetRoom==currentRoom){
    cout<<"Destination location cannot be current location"<<endl;
    return false;
}
//make constants that will help later in game 
const int routeInvalid=0;
const int routeHorizontal=1;
const int routeStairs=2;
const int routeLift1=3;
const int routeLift2=4;
int routeType=routeInvalid;
char connectionType=CONN_NONE;
//now checking weather horizontal movement is possible or not 
if((targetFloor==currentFloor)&&((targetRoom-currentRoom==1)||(currentRoom-targetRoom==1))){
    int connectionIndex=(targetRoom<currentRoom) ? targetRoom-1 : currentRoom-1;
    connectionType=horizontalConn[currentFloor-1][connectionIndex];
    if(connectionType==CONN_DOOR||connectionType==CONN_WINDOW||connectionType==CONN_LOCKED_DOOR){
        //so declaring that movement will be horizontal
        routeType=routeHorizontal;
    }
}
//now check stair movement 
//stairs operate only between 0 index rooms of all floors
else if (currentRoom==1 && targetRoom==1){
    if((currentFloor==1 &&targetFloor==2)||(currentFloor==2 &&targetFloor==1) || (currentFloor==2 && targetFloor==3)||(currentFloor==3 && targetFloor==2)||(currentFloor==4 && targetFloor==5)||(currentFloor==5 && targetFloor==4)){
        routeType=routeStairs;
    }
    }
//now checking lift one movement
else if(currentRoom==3 && targetRoom==3){
    if((currentFloor==1 && targetFloor==2)||(currentFloor==2 && targetFloor==1)){
        routeType=routeLift1;
    }
}
//now checking movement by left2
else if(currentRoom==4  && targetRoom==4){
    bool validMove=(currentFloor==2 || currentFloor==4 || currentFloor==5)&&(targetFloor==2 || targetFloor==4 || targetFloor==5);
    if(validMove){
        routeType=routeLift2;
    }

}
//reject nonexsistent routes
if(routeType==routeInvalid){
    cout<<"No direct route exsist between routes ";
    return false;
} 
// now check if inventory items make movement possible 
bool hasMasterKey=false;
bool hasliftPass=false;
bool hasemergencyCard=false;
for(int i=0;i<inventoryCount;i++){
    if(inventory[i]==ENT_MASTER_KEY){
        hasMasterKey=true;
    }
    if(inventory[i]==ENT_LIFT_PASS){
        hasliftPass=true;
    }
    if(inventory[i]==ENT_EMERGENCY_CARD){
        hasemergencyCard=true;
    }
}
//lock door requirnment
if(routeType==routeHorizontal && connectionType==CONN_LOCKED_DOOR && !hasMasterKey){
    cout<<"Master Key required"<<endl;
    return false;
}
//lift two requirnment checks
bool emergencyTrip=false;
if(routeType==routeLift2){
    if(lift2Operational){
        if(!hasliftPass){
            cout<<"Lift pass required."<<endl;
            return false;
        }
    }
    else{
        if(!hasemergencyCard){
            cout<<"Emergency Access Card Required";
            return false;
        }
        if(emergencyTripsLeft<=0){
            cout<<"no emergency trips remaining."<<endl;
            return false;
        }
        emergencyTrip=true;
    }
}  
//now implementation of movement costs here
int energyCost=COST_MOVE_ORDINARY_ENERGY;
int timeCost=COST_MOVE_ORDINARY_TIME;
if(routeType==routeHorizontal && connectionType==CONN_WINDOW){
    energyCost=COST_MOVE_WINDOW_ENERGY;
    timeCost=COST_MOVE_WINDOW_TIME;
}
else if(routeType==routeLift1 || routeType==routeLift2){
    energyCost=COST_MOVE_LIFT_ENERGY;
    timeCost=COST_MOVE_LIFT_TIME;
}
//now we have to check resourcses before making changes to make code defensive
if(energy< -energyCost || time<-timeCost){
    cout<<"insufficient energy or time."<<endl;
    return false;
}
//now atlast apply the movement 
time+=timeCost;
energy+=energyCost;
if(emergencyTrip){
    emergencyTripsLeft--;
}
currentFloor=targetFloor;
currentRoom=targetRoom;

cout<<"Movement successfull"<<endl;
cout<<"Current Location : Floor "<<currentFloor<<" "<<currentRoom<<endl;
if(emergencyTrip){
    cout<<"Emergency Trips remianing: "<<emergencyTripsLeft<<endl;
}
return true;
}

//function 3-
bool handleInspectCommand(int currentFloor, int currentRoom,
                          int& energy, int& time,
                          const char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                          int entityCount,
                          bool objectives[NUM_OBJECTIVES], int& score){
//validation of current location parameter to function 
if(currentFloor<1 || currentFloor>NUM_FLOORS || currentRoom>ROOMS_PER_FLOOR ||currentRoom<1){
    cout<<"Invalid Current Location";
    return false;
}
//now validating entity count 
if(entityCount<0 || entityCount>MAX_ENTITIES_PER_ROOM){
    cout<<"Invalid room data."<<endl;
    return false;
}
//validating all avalible sources we have 
if(energy<-COST_INSPECT_ENERGY || time<-COST_INSPECT_TIME){
    cout<<"Insufficient energy or time ! ";
    return false;
}
//divide room entities to three variable so it will be easy to detech them 
int usefulObjects=0;
int cluesfound=0;
int livingEntities=0;

for(int i=0;i<entityCount;i++){
    char entity=roomEntities[currentFloor-1][currentRoom-1][i];
    if(entity==ENT_CLUE_7 || entity==ENT_CLUE_4 || entity==ENT_CLUE_2){
        cluesfound+=1;
    }
    else if(entity==ENT_CAT || entity==ENT_PASSENGER){
        //living objects e.g cat / passenger
        livingEntities+=1;
    }
    else if(entity!=ENT_NONE){
        usefulObjects++;
    }
}
//now displaying game player the results
cout<<"\n";
cout<<setfill('=')<<setw(10)<<" ROOM INSPECTION "<<setfill('=')<<setw(10)<<"";
cout<<"Location: Floor "<<currentFloor<<", Room "<<currentRoom<<endl;
//checking if in room no entity is found 
if(entityCount==0){
    cout<<"This Room appears to be empty. "<<endl;
}
else{
    for(int i=0;i<entityCount;i++){
        char entity=roomEntities[currentFloor-1][currentRoom-1][i];
        if(entity!=ENT_NONE){
            //using function to display name to player 
            cout<<" "<<getEntityName(entity)<<endl;
        }
    }
}
//now displaying the entities we find in room 
cout<<'\n';
cout<<"Inspection Findings"<<endl;
cout<<"Clues Found: "<<cluesfound<<endl;
cout<<"Useful objects: "<<usefulObjects<<endl;
cout<<"Living entities: "<<livingEntities<<endl;
//now appling all inspection costs like player inspect room so we will deduct his energy and time accordingly
energy+=COST_INSPECT_ENERGY;
time+=COST_INSPECT_TIME;

cout<<"\nInspection completed successfully ."<<endl;
cout<<"Energy Remaining: "<<energy<<endl;
cout<<"Time remaining: "<<time<<endl;
cout<<setfill('=')<<setw(40)<<endl;
//everything fine then return true
return true;
}
//function 4-----------------------------------------------------------------------------------------------------------------------------------------
//this function will help to collect the enetity 
bool handleCollectCommand(char itemSymbol, int currentFloor, int currentRoom,
                          int& energy, int& time, int& score,
                          char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                          int& entityCount,
                          char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                          bool objectives[NUM_OBJECTIVES]){
    //now firstly validate the parameters of functions
    if(currentFloor<1 || currentFloor>NUM_FLOORS|| currentRoom<1 || currentRoom>ROOMS_PER_FLOOR|| entityCount<0 || entityCount>MAX_ENTITIES_PER_ROOM || inventoryCount<0 || inventoryCount>MAX_INVENTORY_CAPACITY || energy<0 || time<0 || score<0 || score>MAX_SCORE){
        //displaying error
        cout<<"Invalid collection data."<<endl;
        return false;
    }
    //first of all find the index of entity where it is present we used it in later to remove that item from room
    int itemPosition=-1;
    for(int i=0;i<entityCount;i++){
        if(roomEntities[currentFloor-1][currentRoom-1][i]==itemSymbol){
            itemPosition=i;
            break;
        }
    }
    if(itemPosition==-1){
        cout<<"Invalid Item ! This entity is not present in current room";
        return false;
    }
    //using flag
    bool collectable=false;
    //now checking the weather we can collect this entity or not 
    if(itemSymbol==ENT_MASTER_KEY || itemSymbol==ENT_ENERGY_PACK || itemSymbol==ENT_EMERGENCY_TOOLKIT || itemSymbol==ENT_CLUE_2 || itemSymbol==ENT_CLUE_7 || itemSymbol==ENT_CLUE_4 || itemSymbol==ENT_FUSE || itemSymbol==ENT_LIFT_PASS || itemSymbol==ENT_MEDICAL_KIT || itemSymbol==ENT_EMERGENCY_CARD || itemSymbol==ENT_CABINET_TRAIN_KEY || itemSymbol==ENT_CAT_CARRIER || itemSymbol==ENT_OBSERVATION_TOKEN){
        //if entites are from above list then it is collectable
        collectable=true;
    }
    //to add energy and time packets altough inventory is full beacus ethose have nothing to do with inventory 
    bool automaticItem = (itemSymbol==ENT_ENERGY_PACK ||itemSymbol==ENT_EMERGENCY_TOOLKIT || itemSymbol==ENT_OBSERVATION_TOKEN);
    //to creject entities not in our collection list
    if(!collectable){
    cout<<"This entity cannot be collected directly."<<endl;
    return false;
    }
    if(inventoryCount>=MAX_INVENTORY_CAPACITY && !automaticItem){
        cout<<"Your inventory donot have space"<<endl;
        return false;
    }
    //checking energy and time if player has enough to bear to collect entity
    if(energy<-COST_ITEM_PICKUP_ENERGY || time<-COST_ITEM_PICKUP_TIME){
        cout<<"Not Enough energy or time to collect this item."<<endl;
        return false;
    }
    if(itemSymbol==ENT_CABINET_TRAIN_KEY && !objectives[OBJ_CABINET_UNLOCKED]){
    cout<<"Unlock the cabinet first."<<endl;
    return false;
    }
    if(itemSymbol==ENT_ENERGY_PACK){
       energy+=BONUS_ENERGY_PACK;
       if(energy>MAX_ENERGY)
            energy=MAX_ENERGY;
    }
    else if(itemSymbol==ENT_EMERGENCY_TOOLKIT){
        energy+=BONUS_EMERGENCY_TOOLKIT;
        if(energy>MAX_ENERGY)
            energy=MAX_ENERGY;
    }
    else if(itemSymbol==ENT_OBSERVATION_TOKEN){
       time+=BONUS_OBSERVATION_TOKEN;
       if(time>MAX_TIME)
           time=MAX_TIME;
    }
    //after all checks pass we are adding the entity to next avalible inventory place
    if(itemSymbol!=ENT_ENERGY_PACK && itemSymbol!=ENT_EMERGENCY_TOOLKIT &&    itemSymbol!=ENT_OBSERVATION_TOKEN){
    inventory[inventoryCount]=itemSymbol;
    inventoryCount++;
    }
    //now when user collect it we have to remove the item from room by shifting later entities
    for(int i=itemPosition;i<entityCount-1;i++){
        roomEntities[currentFloor-1][currentRoom-1][i]=roomEntities[currentFloor-1][currentRoom-1][i+1];
    } 
    //entitycount substraction now
    entityCount--;
    roomEntities[currentFloor-1][currentRoom-1][entityCount]=ENT_NONE;
    //now if the entity belongs to any objective we will update that objective status
    if(itemSymbol == ENT_FUSE){
    objectives[OBJ_FUSE_COLLECTED]=true;
    }
    else if(itemSymbol==ENT_MASTER_KEY){
        objectives[OBJ_MASTER_KEY]=true;
        score+=SCORE_KEY_COLLECT;
    }
    else if(itemSymbol==ENT_CLUE_4){
        objectives[OBJ_CLUE_4]=true;
        //this one also has score to complete objective so 
        score+=SCORE_CLUE_FOUND;
    }
    else if(itemSymbol==ENT_CLUE_2){
        objectives[OBJ_CLUE_2]=true;
        score+=SCORE_CLUE_FOUND;
    }
    else if(itemSymbol==ENT_CLUE_7){
        objectives[OBJ_CLUE_7]=true;
        score+=SCORE_CLUE_FOUND;
    }
    else if(itemSymbol==ENT_LIFT_PASS){
        objectives[OBJ_LIFT_PASS]=true;
        score+=SCORE_LIFT_PASS;
    }
    else if(itemSymbol==ENT_CABINET_TRAIN_KEY){
        objectives[OBJ_TRAIN_KEY]=true;
        score+=SCORE_TRAIN_KEY;
    }
    else if(itemSymbol==ENT_CAT_CARRIER){
        objectives[OBJ_CAT_CARRIER]=true;
    }
    //now collecting item also apply energy /time cost so applying it
    //beause they are declared -ve in config so add will automaticaaly subtract 
    energy+=COST_ITEM_PICKUP_ENERGY;
    time+=COST_ITEM_PICKUP_TIME;
    //now keeping score within bound 
    if(score>MAX_SCORE){
        score=MAX_SCORE;
    }
    //now displaying player the results
    cout<<getEntityName(itemSymbol)<<"Collected Successfully !"<<endl;
    cout<<"Energy Remaining : "<<energy<<endl;
    cout<<"Time Remaining : "<<time<<endl;
    cout<<"Score: "<<score<<endl;
    //everything goes fine then 
    return true;
}
//function 5-----------------------------------------------------------------------------------------------------------------------------------------
bool handleUseCommand(char itemSymbol, int currentFloor, int currentRoom,
                      int& energy, int& time, int& score,
                      char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                      bool objectives[NUM_OBJECTIVES]){
if(inventory==nullptr || objectives==nullptr){
    cout<<"Invalid Game Data"<<endl;
    return false;
}
//again validating all functions parameter
if(currentFloor<1 || currentFloor>NUM_FLOORS || currentRoom>ROOMS_PER_FLOOR || currentRoom<1 || energy<0 || time<0 || score<0 || score>MAX_SCORE ||inventoryCount<0 || inventoryCount>MAX_INVENTORY_CAPACITY){
    cout<<"Invalid Game Data."<<endl;
    return false;
}
//finding the selected item that is in inventory
int ItemPosition=-1;
for(int i=0;i<inventoryCount;i++){
    if(inventory[i]==itemSymbol){
        ItemPosition=i;
        break;
    }
}
if(ItemPosition==-1){
    cout<<"You donot have this item"<<endl;
    return false;
}
//determine where item must be used in current foor or current room
int requiredFloor=0;
int requiredRoom=0;
if(itemSymbol==ENT_MEDICAL_KIT){
    requiredFloor=4;
    requiredRoom=1;
}
else if(itemSymbol==ENT_CAT_CARRIER){
    requiredFloor=3;
    requiredRoom=3;
}
else if(itemSymbol==ENT_FUSE){
    requiredFloor=3;
    requiredRoom=4;
}
else{
    cout<<"This item cannot be used."<<endl;
    return false;
}
//now checking weather weather player in required room 
if(currentFloor!=requiredFloor || currentRoom!=requiredRoom){
    cout<<"You are not in correct room to use this item";
    return false;
}

//now preventing completing the same objective twice.
if((itemSymbol==ENT_MEDICAL_KIT && objectives[OBJ_PASSENGER_RESCUED]) || (itemSymbol==ENT_CAT_CARRIER && objectives[OBJ_CAT_SECURED]) || (itemSymbol==ENT_FUSE && objectives[OBJ_FUSE_INSTALLED])){
    cout<<"This objective already has been completed"<<endl;
    return false;
}
//check energy and time before performing action
if(energy<-COST_ITEM_USE_ENERGY||time<-COST_ITEM_USE_TIME){
    cout<<"Not enough energy or time ";
    return false;
}
//now after every possible check performing selecting item action 
if(itemSymbol==ENT_MEDICAL_KIT){
    objectives[OBJ_PASSENGER_RESCUED]=true;
    //make changes to score accordingly
    applyScoreDelta(score,100);
    cout<<"Passenger Secured Successfully !"<<endl;
}
else if(itemSymbol==ENT_CAT_CARRIER){
    objectives[OBJ_CAT_SECURED]=true;
    applyScoreDelta(score,100);
    cout<<"Cat Secured Successfully !"<<endl;
}
else if(itemSymbol==ENT_FUSE){
    if(!installFuseControlPanel(inventory,inventoryCount,objectives,score)){
        cout<<"Fuse installation failed."<<endl;
        return false;
    }
    cout<<"Fuse installed successfully! "<<endl;
    cout<<"Lift 2 is now operational"<<endl;
}
//now removing cat carrier and medical kit from inventory fuse is already removed by installfusecontrolpanel
if(itemSymbol!=ENT_FUSE){
    for(int i=ItemPosition;i<inventoryCount-1;i++){
        inventory[i]=inventory[i+1];
    }
    inventoryCount-=1;
    //replacing last item of array that we moved previously to left with ent_none 
    inventory[inventoryCount]=ENT_NONE;
}
//now applying resource deduction by using entities 
energy+=COST_ITEM_USE_ENERGY;
time+=COST_ITEM_USE_TIME;

//now displaying updated information 

cout<<"Energy remaining: "<<energy<<endl;
cout<<"Time remaining: "<<time<<endl;
cout<<"Current score: "<<score<<endl;
//after everything fine return true;
return true;
}
bool handleUnlockCabinetCommand(const char* code, int currentFloor, int currentRoom,
                                bool objectives[NUM_OBJECTIVES], int& score,
                                char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                                int& entityCount){
//again first valiating function parameters 
if(code==nullptr || roomEntities==nullptr || objectives==nullptr || currentFloor<1 || currentFloor>NUM_FLOORS || entityCount<0 || entityCount>MAX_ENTITIES_PER_ROOM || score<0 || score>MAX_SCORE || currentRoom<1 || currentRoom>ROOMS_PER_FLOOR){
    cout<<"Invalid game data. "<<endl;
    return false;
}
//now checking room and floor are valid where we can unlock the cabinet
if(currentFloor!=5 || currentRoom!=1){
    cout<<"the cabinet is located on the floor 5 in room 1. "<<endl;
    return false;
}
//checking objective already not completed to prevent uncloking already unlock cabniet
if(objectives[OBJ_CABINET_UNLOCKED]){
    cout<<"The cabniet is already unlocked."<<endl;
    return false;
}
//confirming weather all clues have been collected then allow unlocking cabniet
if(!objectives[OBJ_CLUE_4]||!objectives[OBJ_CLUE_2]||!objectives[OBJ_CLUE_7]){
    cout<<"You have not collect all Three clues yet. "<<endl;
    return false;
}
//now we have to verify the code entered 
if(!verifyCabinetCode(code,objectives)){
    cout<<"Incorrect cabniet code."<<endl;
    return false;
}
//now checking weather train key is laready in same room or not 
int keyDetected=-1;
for(int i=0;i<entityCount;i++){
    if(roomEntities[4][0][i]==ENT_CABINET_TRAIN_KEY){
        keyDetected=i;
        break;
    }
}
//represent train key if it is not in room .
if(keyDetected==-1){
    if(entityCount>=MAX_ENTITIES_PER_ROOM){
        cout<<"There is no space to reveal Train key."<<endl;
        return false;
    }
    roomEntities[4][0][entityCount]=ENT_CABINET_TRAIN_KEY;
    entityCount+=1;
}
//updating the objective
objectives[OBJ_CABINET_UNLOCKED]=true;
//now adding bonus to resources because unlocking cabniet has bonus
applyScoreDelta(score, 30);
//now display to player
cout<<"Correct Code! "<<endl;
cout<<"Cabniet unlock successfully!"<<endl;
cout<<"The train key is avalible in this room."<<endl;
cout<<"You earned 30 points."<<endl;
cout<<"Current Score: "<<score<<endl;
return true;
}
bool checkEndConditions(int energy, int time, const bool objectives[NUM_OBJECTIVES]){
    //first of all validating functions initial parameters 
    if(objectives==nullptr){
        cout<<"Invalid game state."<<endl;
        return true;
    }
    //now we also have to checking if player has alredy won or not 
    if(objectives[OBJ_MISSION_WON]){
        cout<<"\n";
        cout<<setfill('=')<<setw(40)<<endl;
        cout<<setfill(' ')<<setw(10)<<"Mission Completed!"<<endl;
        cout<<setfill('=')<<setw(40);
        cout<<"You Successfully prepared and boarded the Train!"<<endl;
        //everything fine then
        return true;
    }
    //now checking weather function is already been failed or not 
    if(objectives[OBJ_MISSION_FAILED]){
        cout<<"\n";
        cout<<setfill('=')<<setw(40)<<endl;
        cout<<setfill(' ')<<setw(10)<<"Mission Failed!"<<endl;
        cout<<setfill('=')<<setw(40);
        return true;
    }

    //now checking resources have entered fail state or not
    if(energy<=0 || time<=0){ 
       if(energy<=0 && time<=0){
          cout<<"Both energy and time has been exhausted."<<endl;
        }
        else if(energy<=0){
           cout<<"you have run out of energy."<<endl;
        }
        else{
            cout<<"You have run out of time";
        }
        cout<<"Your rescue mission has ended"<<endl;
        return true;
        }
        //if no ending condition false the nwill return false to indentity that keep game running 
    return false;
    }



























