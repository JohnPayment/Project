#include "Creep.h"

Creep::Creep(const int& teamID, const int& hp, const int& xPos, const int& yPos, const int& lane, const int& type):
 MobileUnit(teamID, hp, xPos, yPos){
    //based on creep type, init attack/move related things
	//if type == x
		//call function to init type x parameters
		
	//etc.
}

int Creep::getLane(){
    return lane_;
}

int Creep::getType(){
    return type_;
}

Point* Creep::getPath(){
    return pPath;
}

Point* Creep::getNext(){
    return pPath+1; 
    //get the next point in the path but don't 
    //increment that actual current value
}

void Creep::setLane(const int& lane){
    lane_ = lane;
}

void Creep::setType(const int& type){
    type_ = type;
}
void Creep::setPath(Point *pt){
    pPath = pt;
}

void Creep::savePoint(Point &pt){
    pSaved = &pt;
}

void Creep::Run(){
    //code to run the AI
    //use a timer so that everything is synchronized
}

/* To test to see if this class compiles type:
 * g++ -Wall -DTESTCOMPILE Creep.cpp */
#ifdef TESTCOMPILE

int main() {
    return 0;
}

#endif
