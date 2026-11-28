#include "switches.h"
#include "util.h"
// function 1----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool canUseLift1(int fromFloor, int toFloor){
    //first of all floor validation here
if(fromFloor>=NUM_FLOORS || fromFloor<0 || toFloor>=NUM_FLOORS || toFloor<0 ){
    return false;
}
//lift 1 operate between floor one and floor two so therefore lift logic will be
if((fromFloor==0 && toFloor==1) ||(fromFloor==1 && toFloor==0)){
    return true;
}
//automatically return false if above condition fails
return false;
}
// function 2-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool canUseLift2(int fromFloor, int toFloor, bool lift2Operational, int emergencyTripsLeft){
    //first check floor validations
if (fromFloor>=NUM_FLOORS || fromFloor<0 || toFloor>=NUM_FLOORS || toFloor<0){
    return false;
}
//check weather floors are those where lift can even operate? 
if((fromFloor==1 && toFloor==3) || (fromFloor==1 && toFloor==4) || (fromFloor==3 && toFloor==1) || (fromFloor==3 && toFloor==4) || (fromFloor==4 && toFloor==1) || (fromFloor==4 && toFloor==3)){
    //checking weather lift is operational or we have emergency tripsleft that can make lift 2 functional
    if(lift2Operational || emergencyTripsLeft>0){
        return true;
    }
}
//no condition satisfied then 
return false;
}
//function 3-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool unlockFloor3Door(const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount){
    //validation first
    if (inventory==nullptr){
        return false;
    }
    if(inventoryCount>MAX_INVENTORY_CAPACITY || inventoryCount<0){
        return false;
    }
    bool key=false;
    for(int i=0; i<inventoryCount;i++){
        if(inventory[i]==ENT_MASTER_KEY){
            key=true;
        }
    }
    if(key){
       return true;
    }
    //reject all other casses
    else{
        return false;
    }
}