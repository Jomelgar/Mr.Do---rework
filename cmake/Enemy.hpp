#pragma once

#include "Direction.hpp"
#include <string>
#include <random>
#include <ctime>

#define _GRASS "\x7"
#define _ENEMY "\x3"
#define _ENEMY_SPAWNER "\x5"
#define _APPLE "\x10"
#define _CHERRY "\x9"
using std::string;

class Enemy
{
    public:
    Enemy() : X(0), Y(0) {}
    Enemy(int x, int y) : X(x), Y(y) {}
    
    void setDirection(Direction pos){dir = pos;}
    void changeDirection();
    void setX(int x){X = x;}
    void setY(int y){Y = y;}

    bool isThere(int x, int y){return Y == y && x == X;}
    bool isUntouchable(string space);
    int getX(){return X;}
    int getY(){return Y;}
    Direction getDirection(){return dir;}

    private:
    int X, Y;
    Direction dir = Direction::Null;
    string body = "\x3";
};

void Enemy::changeDirection() 
{
    std::srand(std::time(0));
    int random = std::rand() % 4;

    switch(random)
    {
        case 0:
            setDirection(Direction::Left);
        break;
        case 1:
            setDirection(Direction::Right);
        break;
        case 2:
            setDirection(Direction::Up);
        break;
        case 3:
            setDirection(Direction::Down);
        break;
    }
}

bool Enemy::isUntouchable(string space)
{
    return space == _GRASS || space == _ENEMY_SPAWNER || space == _ENEMY || space == _APPLE || space == _CHERRY;
}
