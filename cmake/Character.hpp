#pragma once

#include "Direction.hpp"
#include <string>

class Character
{
    public:
    Character(int x,int y) : X(x), Y(y), dir(Direction::Null){}
    void setX(int x){X = x;}
    void setY(int y) {Y = y;}
    void setDirection(Direction d) {dir = d;}

    bool isUntouchable(std::string space);
    int getX(){return X;}
    int getY(){return Y;}
    Direction getDirection(){return dir;}
    const std::string BODY  = "\x2\x1";

    private:
    int X,Y;
    Direction dir;
};

bool Character::isUntouchable(std::string space)
{
    if(space == "\x5" || space == "\x10")
    {
        return true;
    }

    return false;
}