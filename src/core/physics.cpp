#include "physics.h"
#include "util.h"

// Movement validation and resource adjustments without structures
// this function is only checking the connection between rooms of same floor thats why we are not not adding other conditions to it like staircases movement and lift movement amoung floors 
bool isDirectlyConnected(int fromF, int fromR, int toF, int toR,
                         const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1],
                         char& outConnType){
//declaring the initial connection to be none so we cann find fresh connection as a safety measure
outConnType=CONN_NONE;
//checking weather ptr address of horizontal array is even valid or not ?
if(horizontalConn==nullptr){
    return false;
}
// to check even if given from and to rooms or floors indexes are even possible or not 
//feature of defensive cooding
if(fromF>NUM_FLOORS-1 || fromF<0 || toF>NUM_FLOORS-1 || toF<0 || fromR>ROOMS_PER_FLOOR-1 || fromR<0 || toR>ROOMS_PER_FLOOR-1 || toR<0){
    return false;
}
// to reject rooms that are not on same floor 
if(fromF!=toF){
    return false;
}
//to reject rooms that are not adjecent
int difference= fromR-toR;
// absolute type thing here 
if(difference!=1 && difference!=-1){
    return false;
}
//we need low room index beacuse horizontal connection array uses lower room index to check connection between previous room and next room 
int conIndex=(fromR>toR) ? toR : fromR;
//instead of storing directly to outconntype we store this in initial variable to prevent storing the invalid connection in original pointer variable;
char connection=horizontalConn[fromF][conIndex];
//returning false if the connection detected is not even valid as a safety measure 
if (connection!=CONN_SOLID_WALL && connection!=CONN_DOOR && connection!=CONN_WINDOW && connection!=CONN_LOCKED_DOOR && connection!=CONN_NONE){
    return false;
}
outConnType=connection;
//return true if only physical movement possible through these ? 
//even solid wall be rejected beacuse it can also not be considered as valid physical movement condition.
return outConnType==CONN_DOOR || outConnType==CONN_WINDOW || outConnType==CONN_LOCKED_DOOR;
}
bool canTraverse(int targetFloor, int targetRoom, char connType,
                 const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount,
                 int emergencyTripsLeft, bool lift2Operational){

//validating target floor and target room
if(targetFloor>NUM_FLOORS-1 || targetFloor<0 || targetRoom>ROOMS_PER_FLOOR-1 || targetRoom<0){
    return false;
}
//checking inventory has valid address or not ?
if(inventory==nullptr){
    return false;
}
//inventorycount validation as safety measure
if(inventoryCount<0 || inventoryCount>MAX_INVENTORY_CAPACITY){
    return false;
}
//checking door and windows connection and allowing movement 
if(connType==CONN_DOOR || connType==CONN_WINDOW){
    return true;
}
else if(connType==CONN_LOCKED_DOOR){
    //checking weather player has master key or not ? 
    if(hasItem(inventory,inventoryCount,ENT_MASTER_KEY)){
        return true;
    }
    else{
        return false;
    }
}
//checking weather lift2 works on the current room
 bool isLift2Room = (targetRoom ==3) && (targetFloor ==1 || targetFloor ==3 || targetFloor ==4);
//check only if there is no possible connection between rooms earlier 
    if (isLift2Room && connType == CONN_NONE) {
        // Normal Lift 2 access
        if (lift2Operational) {
            return hasItem(inventory, inventoryCount, ENT_LIFT_PASS);
        }
        // Emergency Lift 2 access
        return (emergencyTripsLeft >0) && hasItem(inventory, inventoryCount, ENT_EMERGENCY_CARD);
}
//none cases work so then return false;
return false;
}
