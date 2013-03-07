#ifndef PLAYER_H
#define PLAYER_H

#include "MobileUnit.h"

class Player: public MobileUnit{
public:
    Player(const int& teamID, const int& playerID, const int& hp, const int& xPos, const int& yPos, const int& type);

    int getID();

    int getType();

    void setType(const int& type);

    virtual void Run();

private:
    int playerID_;
    int playerType_;
};

#endif