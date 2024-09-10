#pragma once

#include<string>
#include "Direction.hpp"

using std::string;

int COOLDOWN = 3000000000;

class Proyectile
{
public:

    Proyectile() : dir(Direction::Null), X(0), Y(0), active(false){}
    Proyectile(int x, int y, Direction mov) : dir(mov), X(x), Y(y), active(false){}
    bool isTimerReady()
    {
        if(timer <= 0) 
        {
            timer = COOLDOWN;
            return true;
        }
        setTimer();
        return false;
    }

    void setTimer(){timer--;}
    void setDirection(Direction mov){dir = mov;}
    void setActive(){active = !active;}
    void setX(int x){X = x;}
    void setY(int y){Y = y;}

    int getX(){return X;}
    int getY(){return Y;}
    bool getActive(){return active;}
    Direction getDirection(){return dir;}

    string BODY = "\x6";
private:
    int timer = 0;
    int X, Y;
    bool active;
    Direction dir;
};