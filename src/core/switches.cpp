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

//function 4 
bool installFuseControlPanel(char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                             bool objectives[NUM_OBJECTIVES], int& score){
//validation checking first 
 if ((inventory==nullptr)||(objectives==nullptr)){
    return false;
 }
 //validating inventory count 
 if (inventoryCount>MAX_INVENTORY_CAPACITY || inventoryCount<0){
    return false;
 }
 //score cannot be negative
 if (score<0){
    return false;
 }
 //check weather already complted or not to avoid giving score bonus twice
 if(objectives[OBJ_FUSE_INSTALLED]){
    return false;
 }
 //using flag here
 bool foundFuse=false;
 int fuseIndex=-1;
 for (int i=0;i<inventoryCount;i++){
    if (inventory[i]==ENT_FUSE){
        foundFuse=true;
        fuseIndex=i;
        break;
    }
 }
 if(!foundFuse){
    return false;
 }
else{
    //process to remove fuse from inventory as it is going to be used
    for(int i=fuseIndex;i<inventoryCount-1;i++){
        inventory[i]=inventory[i+1];
    }
    inventoryCount-=1;
    //assigning null value to left most index that will be fuse there 
    inventory[inventoryCount]='\0';
    //updating objectives 
    objectives[OBJ_FUSE_INSTALLED]=true;
    //activating lift2 
    objectives[OBJ_LIFT2_ACTIVE]=true;
    //score reward for activating lift
    score+=SCORE_INSTALL_FUSE;
    //after full succcessfull process
    return true;
}}
//function 5- 
bool verifyCabinetCode(const char* enteredCode, const bool objectives[NUM_OBJECTIVES]){
if (enteredCode==nullptr || objectives==nullptr){
    return false;
}
//checking weather all clues obtained or not 
if(!objectives[OBJ_CLUE_2] || !objectives[OBJ_CLUE_4] || !objectives[OBJ_CLUE_7] ){
    return false;
}
const char actualCode[]="427";
//using function mentioned in ultil instead of writing whole code again
if(stringEquals(enteredCode,actualCode)){
    return true;
}
return  false;
}