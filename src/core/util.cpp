#include <string>
#include <iostream>
#include "util.h"
using namespace std;
// Primitive helper utilities, clamping, and screen control
// function 1 
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void trimWhitespace(char* str){
int i=0; //starting index
int j=0; //ending index 
int lenght=0; // record lenght of string
// counting lenght of string 
if (str==nullptr){
    return;
}
while(str[lenght]!='\0'){
    lenght+=1;
}
if (lenght==0){
    return;
}
//updating last index on the basis of lenght
j=lenght-1;
//updating the i based on spaces in string from start
//use ' ' becuase space is a single caracter not a string so we donot use " "
while((str[i]==' ')||(str[i]=='\t')||(str[i]=='\n')||(str[i]=='\r')){
    i++;
}
//updating the j based on spaces in string at end
while((j>=i)&&((str[j]==' ')||(str[j]=='\t')||( str[j]=='\n')|| (str[j]=='\r'))){
    j--;
}
//updating original string provided to function 
int k=0;
for(;i<=j;k++,i++){
   str[k]=str[i];
}
//upper loop add one more value to i so i will be at one more than the last of the string where we also have to add the 
str[k]='\0';
// c++ recognize the end of string when it sees th '\0' this caracter at the end of string otherwise this above loop acts as the set of array by adding the '\0' c++ able to understand that this is string and ends here.
// declaring with "" c++ automactically add the '\0' at the end to recognize the end of string the c++ only memorizes the address of first letter of string and ends the string when it reads the '\0' there.
}
// function 2 ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int clampInt(int val, int minVal, int maxVal){
    // if user accidently enter minval and maxval in wrong parameter so to handle that edge case
if (minVal>maxVal){
    // temporary variable to store minval so that its value cannot be lost 
    int instant=minVal;
    minVal=maxVal;
    maxVal=instant;
}
// use of tenary and apply neccessary condition to keep int in bound 
val=(val>maxVal) ? maxVal : (val<minVal) ?  minVal : val;
return val;
} 
/* function2
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool stringEquals(const char* a, const char* b){
    // if any of the pointer donot have address of reference variable
    if (a==nullptr || b==nullptr){
        return false; // function will end here
    }
    int k=0;
    // a represents address and first caracter address of string it donot know lenght actually
    // *(a+1) will be exactly equal to a[1]
    while(a[k]!='\0' && b[k]!='\0'){
        //comparing strings by string indexing 
        if (a[k]!=b[k]){
            // if only one not matched it will return false and function will end no more comparisions.
            return false;
        }
    k++;
    }
    //compare the caracters when the loop breaks because might one string ends other continue so if both ends then both last caracters will be string ending caracter'\0' so if these match so it will rteurn true because upper already matched we check in loop
    return a[k]==b[k];
}
//function 4 
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int stringLenght(const char* header){
if (header==nullptr){
    return 0;
}
int lenght=0;
while(header[lenght]!='\0'){
    lenght++;
}
return lenght;
}
//function 5
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void printHeader(const char* title){
if (title==nullptr){
return; }
int length=stringLenght(title);
// we want atleast two spaces at the both side of string that is why we use 4
int width=(length + 4 >40) ? (length+4):  40;
// spaces we want to add to bring tittle in exactly middle 
int spaces=(width-length) / 2;
//spaces from right side 
int rightSpaces=width-length-spaces;
//loop for above ==
for (int i=0;i<width;i++){
    cout<<"=";}
cout<<endl;
//loop for middle title 
for (int i=0; i<spaces;i++){
    cout<<" ";}
cout<<title;
for (int i = 0; i < rightSpaces; i++){
    cout << " ";}
cout << endl;
//loop for below ==
for (int i=0;i<width;i++){
    cout<<"=";}
cout<<endl;
}
// function 6
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// function to clear terminal / screen
void clearScreen(){
//   Code	Purpose
//   \033	Escape character
//    [2J	Clear the terminal screen
//    [H	Move cursor to the top-left corner
cout<<"\033[2J\033[H";
}
