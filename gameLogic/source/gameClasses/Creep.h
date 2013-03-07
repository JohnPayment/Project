#ifndef CREEP_H
#define CREEP_H

#include "MobileUnit.h"
#include "Point.h"

class Creep: public MobileUnit{
public:
    Creep(const int& teamID, const int& hp, const int& xPos, const int& yPos, const int& lane, const int& type);

    int getLane();

    int getType();

    Point* getPath();

    Point* getNext();

    void setLane(const int& lane);
    
    void setType(const int& type);

    void setPath(Point *pt);

    void savePoint(Point &pt);

    virtual void Run();

private:
    Point *pPath;
    Point *pSaved;
    int lane_;
    int type_; //the type of the creep, so we can have aoe, etc.
    //others?
};

#endif