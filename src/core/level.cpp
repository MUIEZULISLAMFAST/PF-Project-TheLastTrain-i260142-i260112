#include "level.h"
#include "util.h"
#include <fstream>
#include "state.h"
#include "config.h"
#include <climits>
// Building & Level Parsing using primitive parallel arrays

bool loadBuildingMap(const char* filename,
                     char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1],
                     char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                     int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR]){
    // checking weather arrays pointers carry some address or not 
    if((horizontalConn==nullptr) || (roomEntities==nullptr)|| (entityCounts==nullptr)){
        return false;
    }
    // initialized the temporary arrays to make code defensive to prevent any wrong or mistaken change to original arrays provided to function
    // Temporary arrays protect the original destination arrays
    // from partially loaded or invalid map data.
    // we will make changes to riginal arrays after validation
    char tempHorizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1];
    char tempRoomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM];
    int tempEntityCounts[NUM_FLOORS][ROOMS_PER_FLOOR];
    // initializing with none values provided in stat.h beacuse we want our code of game too be safe like in file reading of building.txt if some connection or entity got missing out game should not be crahed we will initiate game with none value provided to us in header files.
    //2d array structure that we are assigning the entitycounts to 0 and also room entities to none one 
    for(int i=0;i<NUM_FLOORS;i++){
        // each floor then have 4 rooms
        for(int j=0 ;j<ROOMS_PER_FLOOR;j++){
            tempEntityCounts[i][j]=0;
            //each room then have maximum of 5 entities
            for(int k=0;k<MAX_ENTITIES_PER_ROOM;k++){
                tempRoomEntities[i][j][k]=ENT_NONE;
            }   
        }
    }
    // 2d structure of horizontal connections among rooms 
    for(int i=0;i<NUM_FLOORS;i++){
        // four rooms meaning between rooms there will be 3 connections in total
        for(int j=0;j<ROOMS_PER_FLOOR-1;j++){
            tempHorizontalConn[i][j]=CONN_NONE;
        }
    }
    // now below we are going to open building.txt and read data from there so we have to include library that is fstream
    //     nullptr to check validity of address
    //    '\0' to check valid pointer but empty string
    //     empty string has first letter of '\0'
    if (filename==nullptr || filename[0]=='\0'){
        return false;
    }
    //fstream beacuse we want to read file only 
    ifstream file(filename);
    // check weather file fails or not there are more methods also but i think it should be fine to use here
    if (file.fail()){
        //close function safely if file fails to open
        return false;
    }
    // initiate all rooms with imposiible index of -2
    // this means room lables are not declared it 
    // this is our tracing array like at last we check from it weather every floor-room location finds and if find it will give us the starting index of every floor room it does'nt store the room or floor index like we imagine in ray it represents text file building.txt line starting location of text etc F5-R1
    int roomLabelPositions[NUM_FLOORS][ROOMS_PER_FLOOR];
    for (int i=0;i<NUM_FLOORS;i++){
        for (int j = 0; j < ROOMS_PER_FLOOR; j++){
            roomLabelPositions[i][j] = -2;
        }
    }
    //
    //     now reding data from building.txt that was already given in zip file of project 
    //     getline() itself places the '\0' terminator in your array text file originally not possess it
    //     The secret is that getline() doesn't search for '\0' in the text file. It searches for a newline character '\n'
    char line[MAX_LINE_LEN];
    int previousFloor=-1;
    //to check previous line was room label or not 
    bool previousLineStatus=false;
    while(file.getline(line,MAX_LINE_LEN)){
        if (previousLineStatus) {
            if ((previousFloor<0)||(previousFloor>=NUM_FLOORS)){
                return false;
            }
            int lineLength = 0;
            while (line[lineLength] != '\0') {
                lineLength++;
            }
            for (int i=0; line[i]!='\0';i++){
            int roomIndex = -1;
            char current = line[i];
            bool Entity_validator = (current==ENT_MASTER_KEY) || (current==ENT_ENERGY_PACK) || ( current==ENT_EMERGENCY_TOOLKIT) || (current==ENT_CLUE_4) ||(current==ENT_FUSE) ||(current==ENT_CAT) ||(current==ENT_CONTROL_PANEL) ||(current==ENT_LIFT_PASS) ||(current==ENT_PASSENGER) ||(current==ENT_MEDICAL_KIT) ||(current==ENT_EMERGENCY_CARD) ||(current==ENT_CLUE_2) ||(current==ENT_CLUE_7) ||(current==ENT_CABINET_TRAIN_KEY) ||(current==ENT_CAT_CARRIER) ||(current==ENT_OBSERVATION_TOKEN);

        if (Entity_validator){
            for (int j=0;j<ROOMS_PER_FLOOR;j++){
               int labelPosition = roomLabelPositions[previousFloor][j];
               if (labelPosition < 0) {
                    return false;
                }
               if (labelPosition >= lineLength) {
                   return false;
                }
            int leftBoundary = -1;
            int rightBoundary = -1;

            // Find nearest wall on the left.
            for (int k = labelPosition - 1; k >= 0; k--) {
                if (line[k] == '|') {
                    leftBoundary = k;
                    break;
        }
        }
        // Find nearest wall on the right.
        for (int k = labelPosition; k<lineLength; k++) {
            if (line[k] == '|') {
                rightBoundary = k;
                break;
        }
        }
       if (leftBoundary != -1 && rightBoundary != -1 &&
            i > leftBoundary && i < rightBoundary) {
            roomIndex = j;
            break;
        }
}
            if (roomIndex<0 || roomIndex>=ROOMS_PER_FLOOR) {
                return false ;
            }
            
            if (tempEntityCounts[previousFloor][roomIndex]
                >= MAX_ENTITIES_PER_ROOM) {
                return false;
            }
            int position=tempEntityCounts[previousFloor][roomIndex];
            tempRoomEntities[previousFloor][roomIndex][position]=current;
            tempEntityCounts[previousFloor][roomIndex]+=1;
        }
        else if (current != '|' && current != ' ' && current != '\t') {
            return false;
        }
    }
    previousLineStatus = false;
}
        int currentLinePositions[ROOMS_PER_FLOOR];
        for (int i=0;i<ROOMS_PER_FLOOR;i++){
            currentLinePositions[i]=-1;
        }
        int currentFloor=-1;
        // dynamic loop that can automatically check exsistence of F and check is it a valid F by checking exsistence of next four indexes and we also bounded 
        // it is also independent of too much on building.txt b/c our phraser can also work if we put some selected changes to building.txt like if we write floor 1 first on top instead of floor 5 our project will still work on it. 
        for(int i=0;line[i]!='\0';i++){
            if ((line[i]=='F') &&(i+4<MAX_LINE_LEN) && (line[i+1]!='\0') && (line[i+2]!='\0') && (line[i+3]!='\0') && (line[i+4]!='\0') && (line[i+1]<=('0'+NUM_FLOORS) && line[i+1]>='1') && (line[i+2]=='-') &&(line[i+3]=='R') && (line[i+4]<=('0'+ROOMS_PER_FLOOR) && line[i+4]>='1')){
            // actual rooms and floors 
            int floorNo =line[i+1]-'0';
            int roomNo =line[i+4]-'0';
            //for passing to temporary array parameters 
            int floorIndex = floorNo-1;
            int roomIndex = roomNo-1;
            //checking weather in any early loop this room is dectected or not if its not detected then update walue if its alredy detected has something instead of -2 we will return fasle because bulding.txt is not valid has duplicate rooms
            if (roomLabelPositions[floorIndex][roomIndex] != -2){
                return false;
            }

            if (currentFloor==-1){
                currentFloor=floorIndex;
            }
            else if(currentFloor!=floorIndex){
                return false;
            }   
            //storing starting position of each label
            roomLabelPositions[floorIndex][roomIndex] = i;
            // stroring current line positions 
            currentLinePositions[roomIndex]=i;
            // we are here using flag to check weather all the four indexes of starting of f present in currentLinePosition or not 
                }
                }
            bool checkComplete=true;
            for (int i=0 ;i<ROOMS_PER_FLOOR;i++){
                if (currentLinePositions[i]==-1){
                    checkComplete=false;
                    //use break here instead of return beacuse all rows in building.txt donot have what we demand some have empty contenet also thats why
                    break;
                }
            }
            //loop runs only when all  current line positions present in currentlinepositions[]
            if (checkComplete){
                for(int i=0;i<ROOMS_PER_FLOOR-1;i++){
                
                   int start=currentLinePositions[i]+5;
                   int end=currentLinePositions[i+1];
                   //other security measure
                   if (start>=end){
                        return false;
                   }
                   //intiate connection as default 
                   char connection=CONN_NONE;
                   // counting connection so that at end we can check weather loop iteration got sucessfull or not 
                   int connectionCount=0;
                   bool liftDetection=false;
                   // loop between space of two rooms
                   for(int j=start;j<end;j++){
                        char current=line[j];
                   if (current=='L' && j+1<end && (line[j+1]=='1' || line[j+1]=='2')){
                        if (liftDetection) {
                             return false;
                            }
                         liftDetection=true;
                         j++;
                         continue;
                         }
                   if (current=='D' || current=='W' || current=='L' || current=='#'){
                        connection=current;
                        connectionCount+=1;
                   }
                   else if (current != ' ' && current != '\t') {
                        return false;
                    }
            }
            if ((!liftDetection && connectionCount !=1) || (liftDetection && connectionCount !=0)) {
                return false;
            }
            tempHorizontalConn[currentFloor][i]=connection;
        }
        previousLineStatus=true;
        previousFloor=currentFloor;
       
        }
    }
    //checking weather file closes 
    // Check for file-reading errors
    //we have not used one because one is not sufficient here for a defensive program i used all three
    // file.bad()--return false only when there is serious input/output problem
    // file.fail()--return false only when the last operation fails it can fail if eof flag become active(when reach at the end of line)
    // file.eof()--return true when eof flag become active When getline() tries to read beyond the available content.
    //thats why we use && between . that if last operation fails it should not be due to eof flag activation 
    if (previousLineStatus){
    return false;
    }
    if (file.bad() || (file.fail() && !file.eof())){
        return false;
    }
    // making sure that all rooms label are correctly entered if some is missing it will return false
    for (int i=0;i<NUM_FLOORS;i++){
        for (int j=0;j<ROOMS_PER_FLOOR;j++){
            if (roomLabelPositions[i][j]==-2){
                return false;
            }
        }
    }

//now after all validations i want to are coping data to original arrays so that it would be a safety meausre
// copying connections array
for (int i=0;i<NUM_FLOORS;i++){
    for(int j=0;j<ROOMS_PER_FLOOR-1;j++){
        horizontalConn[i][j]=tempHorizontalConn[i][j];
    }
}
//coping entities array
for(int i=0; i<NUM_FLOORS;i++){
    for(int j=0; j<ROOMS_PER_FLOOR;j++){
        for(int k=0;k<MAX_ENTITIES_PER_ROOM;k++){
            roomEntities[i][j][k]=tempRoomEntities[i][j][k];
        }
    }
}
//copying entity counts per room 
for (int i=0;i<NUM_FLOORS;i++){
    for(int j=0 ;j<ROOMS_PER_FLOOR;j++){
        entityCounts[i][j]=tempEntityCounts[i][j];
    }
}
// arrays loaded impormation successfully from building.txt
return true;
}

bool loadInitialState(const char* filename,
                      int& playerFloor, int& playerRoom,
                      int& energy, int& time, int& score,
                      char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                      bool objectives[NUM_OBJECTIVES], int& emergencyTripsLeft){
    if (filename==nullptr || filename[0]=='\0') {
        return false;
    }
    if (inventory==nullptr || objectives==nullptr) {
        return false;
    }
    ifstream file(filename);
    //check weather file open successfully or there is any error
    if(!file.is_open()){
        return false;
    }
    char line[MAX_LINE_LEN];
    bool verifier[17]={false};
    for(int i=0; i<NUM_OBJECTIVES;i++){
    objectives[i] = false;
    }
    inventoryCount=0;
    emergencyTripsLeft=0;
    while(file.getline(line,MAX_LINE_LEN)){
       trimWhitespace(line);
       int k=0;
       string  currentStateVariable;
       string currentStateVariableValue;
       int currentStateVariableValueInt=0;
       bool numericChecker=true;
       bool validLine=false;
       for(int i=0; line[i]!='\0';i++){
          if(line[i]==':'){
            k=i;
            validLine=true;
            break;
        }
        }
        if (validLine){
        for(int j=0;j<k;j++){
            if (line[j]!=' '){
                currentStateVariable+=line[j];
            }
        }
        for (int m=k+1;line[m]!='\0';m++){
            if(line[m]!=' '){
            currentStateVariableValue+=line[m];
            if((line[m]!=' ')&& (line[m]>57 || line[m]<48)){
                numericChecker=false;
            }  
         }
        }
        if (numericChecker){
            for(int i=0;currentStateVariableValue[i]!='\0';i++){
               if(int(currentStateVariableValue[i])<=57 && int(currentStateVariableValue[i])>=48){
                  int tempint= currentStateVariableValue[i]-'0';
                  if (currentStateVariableValueInt > (INT_MAX - tempint) / 10) {
                    return false;
                }
                  currentStateVariableValueInt=currentStateVariableValueInt*10+tempint;
            } 
        }
    }
    if(currentStateVariable=="GAME_STATUS"){
    if(verifier[0] || currentStateVariableValue!="NOT_STARTED")
        return false;
    verifier[0]=true;
    }
   else if(currentStateVariable=="START_ROOM"){
        if(verifier[1] || currentStateVariableValue!="PLAYER_SELECTS")
            return false;
        // Validate the initial player location passed by main().
        if (playerFloor<1 || playerFloor>NUM_FLOORS ||
        playerRoom<1 || playerRoom>ROOMS_PER_FLOOR) {
        return false;
    }
    verifier[1]=true;
    }
    else if(currentStateVariable=="INITIAL_ENERGY"){
        if(verifier[2] || !numericChecker || currentStateVariableValue=="")
           return false;
        if(currentStateVariableValueInt!=INITIAL_ENERGY)
           return false;
        energy=currentStateVariableValueInt;
        verifier[2]=true;
    }
    else if(currentStateVariable == "INITIAL_TIME"){
        if(verifier[3] || !numericChecker || currentStateVariableValue=="")
            return false;
        if(currentStateVariableValueInt!=INITIAL_TIME)
           return false;
        time = currentStateVariableValueInt;
        verifier[3]=true;
    }
    else if(currentStateVariable=="INITIAL_SCORE"){
        if(verifier[4] || !numericChecker || currentStateVariableValue=="")
            return false;
        if(currentStateVariableValueInt!=INITIAL_SCORE)
            return false;
        score=currentStateVariableValueInt;
        verifier[4]=true;
    }
    else if(currentStateVariable=="MAX_ENERGY"){
        if(verifier[5] || !numericChecker || currentStateVariableValue=="")
            return false;
        if(currentStateVariableValueInt != MAX_ENERGY)
            return false;
        verifier[5]=true;
}
    else if(currentStateVariable=="MAX_TIME"){
        if(verifier[6] || !numericChecker || currentStateVariableValue=="")
            return false;
        if(currentStateVariableValueInt!=MAX_TIME)
            return false;
        verifier[6] = true;
    }
    else if(currentStateVariable=="INITIAL_INVENTORY"){
        if(verifier[7] || currentStateVariableValue != "NONE")
            return false;
        inventoryCount = 0;
        for(int i=0;i<MAX_INVENTORY_CAPACITY;i++){
            inventory[i]='\0';
        }
        verifier[7]=true;
        }
    else if(currentStateVariable=="PASSENGER_RESCUED"){
        if(verifier[8])
            return false;
        if(currentStateVariableValue=="YES")
            objectives[OBJ_PASSENGER_RESCUED]=true;
        else if(currentStateVariableValue=="NO")
            objectives[OBJ_PASSENGER_RESCUED]=false;
        else
            return false;
        verifier[8] = true;
    }
    else if(currentStateVariable=="CAT_RESCUED"){
        if(verifier[9])
            return false;
        if(currentStateVariableValue=="YES")
            objectives[OBJ_CAT_SECURED]=true;
        else if(currentStateVariableValue=="NO")
            objectives[OBJ_CAT_SECURED]=false;
        else
            return false;
        verifier[9]=true;
    }
    else if(currentStateVariable=="CLUES_COLLECTED"){
        if(verifier[10] || currentStateVariableValue!="NONE")
            return false;
        objectives[OBJ_CLUE_4]=false;
        objectives[OBJ_CLUE_2]=false;
        objectives[OBJ_CLUE_7]=false;
        verifier[10]=true;
    }
    else if(currentStateVariable=="CABINET_UNLOCKED"){
       if(verifier[11])
           return false;
       if(currentStateVariableValue=="YES")
           objectives[OBJ_CABINET_UNLOCKED]=true;
       else if(currentStateVariableValue=="NO")
            objectives[OBJ_CABINET_UNLOCKED]=false;
        else
           return false;
        verifier[11]=true;
    }
    else if(currentStateVariable=="TRAIN_KEY_COLLECTED"){
        if(verifier[12])
            return false;
        if(currentStateVariableValue=="YES")
            objectives[OBJ_TRAIN_KEY]=true;
        else if(currentStateVariableValue=="NO")
            objectives[OBJ_TRAIN_KEY]=false;
        else
            return false;
        verifier[12]=true;
    }
    else if(currentStateVariable=="FUSE_INSTALLED"){
        if(verifier[13])
            return false;
        if(currentStateVariableValue=="YES")
            objectives[OBJ_FUSE_INSTALLED]=true;
        else if(currentStateVariableValue=="NO")
            objectives[OBJ_FUSE_INSTALLED]=false;
        else
            return false;
        verifier[13]=true;
    }
    else if(currentStateVariable=="LIFT_PASS_COLLECTED"){
        if(verifier[14])
            return false;
        if(currentStateVariableValue=="YES")
            objectives[OBJ_LIFT_PASS]=true;
        else if(currentStateVariableValue=="NO")
            objectives[OBJ_LIFT_PASS]=false;
        else
            return false;
        verifier[14]=true;
    }
    else if(currentStateVariable=="TRAIN_PREPARED"){
        if(verifier[15])
            return false;
        if(currentStateVariableValue=="YES")
            objectives[OBJ_TRAIN_PREPARED]=true;
        else if(currentStateVariableValue=="NO")
            objectives[OBJ_TRAIN_PREPARED]=false;
        else
            return false;
        verifier[15]=true;
    }
    else if(currentStateVariable=="PLAYER_AT_TRAIN"){
        if(verifier[16])
            return false;
        if(currentStateVariableValue!="YES" &&
            currentStateVariableValue!="NO")
            return false;
        verifier[16]=true;
    }
    else{
        return false;
    } 
    }
    else {
    if (line[0]!='\0') {
        return false;
    }
    }
    }
     if(file.fail() && !file.eof()){
        return false;
    }
    for(int i=0;i<17;i++){
        if(!verifier[i]){
            return false;
        }
    }
    return true;
}
bool validateStartingRoom(int startFloor, int startRoom,
                          const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1]);
const char* getEntityName(char entitySymbol);

