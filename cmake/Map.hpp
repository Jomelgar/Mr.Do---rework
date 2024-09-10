#pragma once

#include "TextConsole.h"
#include "Direction.hpp"
#include "Character.hpp"
#include "Proyectile.hpp"
#include "Cherry.hpp"
#include "Apple.hpp"
#include "Enemy.hpp"
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <random>
#include <ctime>


using std::string;

#define POINTS_KILL 500

int SIZE_X = 30, SIZE_Y = 15;

int Character_SPAWN_X = 14, Character_SPAWN_Y = 13;
int ENEMY_SPAWNER_X = 14 , ENEMY_SPAWNER_Y = 7;
string ENEMY_SPAWNER_BODY = "\x5";
string CHARACTER_BODY = "\x4";
string ENEMY_BODY = "\x3";
string GRASS = "\x7";

class Map {
public:
    Map(TextConsole *cons, int _score, int _lifes, int _level);
    Map(TextConsole *cons);
    Map(const Map&) = delete;
    Map& operator=(const Map&) = delete;
    ~Map() {
        running = false;
        if (objectThread && objectThread->joinable()) {
            objectThread->join();
        }
        if (enemyThread && enemyThread->joinable()) {
            enemyThread->join();
        }
        for (int i = 0; i < SIZE_X; i++) {
            delete[] space[i];
        }
        delete[] space;
        con->clearScreen();
    }

    //Operations for Main
    void MoveCharacter(Direction dir);
    void Shoot();

    //Functions
    int getScore(){return score;}
    int getLifes(){return lifes;}
    int getLevel(){return level;}
    bool isGameOver(){return gameOver;}

private:
    //Private Operations
    void enemyLoop();
    void objectLoop();
    void seeEnemies();
    void generateMap();
    void generateCherryApple();
    void toString();
    void printData();
    void restartGame();
    void SeeApples();
    void MoveApple(Apple & app);
    void MoveEnemy(Enemy & en);
    void MoveProyectile();
    int searchEnemyIndex(int x, int y);

    //Attributes
    int score = 0, lifes = 3, level = 1;
    string **space;
    Character character = Character(Character_SPAWN_X, Character_SPAWN_Y);
    Proyectile proyectile;
    TextConsole *con;
    Cherry cherries[8];
    Apple apples[8];
    std::unique_ptr<std::thread> objectThread;
    //Only spawn Enemies
    std::unique_ptr<std::thread> enemyThread;
    bool running = true;
    std::vector<Enemy> Enemies;
    bool gameOver = false;
};

Map::Map(TextConsole *cons, int _score, int _lifes, int _level) : score(_score), lifes(_lifes), level(_level)
{
    //Random Color
    int r = std::rand() % 156;
    int g = std::rand() % 156;
    int b = std::rand() % 156;
    con->setForecolor(CRGBColor(r+100, g+100, b+100));
    
    //Generate Map
    generateMap();
    // Object Spawn
    space[Character_SPAWN_X][Character_SPAWN_Y] = CHARACTER_BODY;

    //Enemy Spawner
    space[ENEMY_SPAWNER_X][ENEMY_SPAWNER_Y] = ENEMY_SPAWNER_BODY;

    //Spawn of cherries and apples
    generateCherryApple();

    //Print map
    toString();
    
    //Print Borders
    for (int i = 0; i <= 30; i++)
    {
        con->setCursor(15,i);
        *con<< "-";
    }
    
    //THE THREAD
    objectThread = std::make_unique<std::thread>(&Map::objectLoop, this);
    enemyThread = std::make_unique<std::thread>(&Map::enemyLoop, this);
}

Map::Map(TextConsole *cons) : con(cons)
{
    generateMap();
    // Object Spawn
    space[Character_SPAWN_X][Character_SPAWN_Y] = CHARACTER_BODY;

    //Enemy Spawner
    space[ENEMY_SPAWNER_X][ENEMY_SPAWNER_Y] = ENEMY_SPAWNER_BODY;

    //Spawn of cherries and apples
    generateCherryApple();

    //Print map
    toString();
    
    //Print Borders
    for (int i = 0; i <= 30; i++)
    {
        con->setCursor(15,i);
        *con<< "-";
    }

    //THE THREAD
    objectThread = std::make_unique<std::thread>(&Map::objectLoop, this);
    enemyThread = std::make_unique<std::thread>(&Map::enemyLoop, this);;
}

void Map::MoveProyectile()
{
    if(proyectile.getActive())
    {
        if(proyectile.getX() <= 0 && proyectile.getDirection() == Direction::Left
            || proyectile.getX() >= SIZE_X-1 && proyectile.getDirection() == Direction::Right
            || proyectile.getY() <= 0 && proyectile.getDirection() ==Direction::Up 
            || proyectile.getY() >= SIZE_Y-1 && proyectile.getDirection() == Direction::Down)
        {
            proyectile.setActive();
            con->setCursor(proyectile.getY(),proyectile.getX());
            *con << " ";
        }
        else
        {
            con->setCursor(proyectile.getY(),proyectile.getX());
            space[proyectile.getX()][proyectile.getY()] = " ";
            *con << space[proyectile.getX()][proyectile.getY()];
                switch(proyectile.getDirection())
                {
                    case Direction::Left:
                        proyectile.setX(proyectile.getX() - 1);
                    break;
                    case Direction::Right:
                        proyectile.setX(proyectile.getX() + 1);
                    break;
                    case Direction::Up:
                        proyectile.setY(proyectile.getY() - 1);
                    break;
                    case Direction::Down:
                        proyectile.setY(proyectile.getY() + 1);
                    break;
                }
                if(space[proyectile.getX()][proyectile.getY()] == ENEMY_BODY)
                {
                    int index = searchEnemyIndex(proyectile.getX(), proyectile.getY());
                    Enemies.erase(Enemies.begin() + index);
                    proyectile.setActive();
                    score += POINTS_KILL;
                    space[proyectile.getX()][proyectile.getY()] = " ";
                    con->setCursor(proyectile.getY(),proyectile.getX());
                    *con << space[proyectile.getX()][proyectile.getY()];
                    printData();
                }
                else if(space[proyectile.getX()][proyectile.getY()] !=  " ")
                {
                    proyectile.setActive();
                }
                else
                {
                    space[proyectile.getX()][proyectile.getY()] = "\x6";
                    con->setCursor(proyectile.getY(),proyectile.getX());
                    *con << space[proyectile.getX()][proyectile.getY()];
                }
        }
    }
}

int Map::searchEnemyIndex(int x, int y)
{
    for (int i = 0; i < Enemies.size(); i++)
    {                    
        if(Enemies[i].isThere(x,y))
        {
            return i;
        }
    }

    return -1;
}

void Map::enemyLoop()
{
    while(running)
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        
        if(Enemies.size() < 4)
        {
            Enemy enemy = Enemy(ENEMY_SPAWNER_X,ENEMY_SPAWNER_Y - 1);
            enemy.changeDirection();
            Enemies.push_back(enemy);
            space[ENEMY_SPAWNER_X][ENEMY_SPAWNER_Y-1] = ENEMY_BODY;
            con->setCursor(ENEMY_SPAWNER_Y-1,ENEMY_SPAWNER_X);
            *con << space[ENEMY_SPAWNER_X][ENEMY_SPAWNER_Y-1];
        }
    }
}

void Map::objectLoop()
{
    while (running) 
    {
        MoveProyectile();
        seeEnemies();
        SeeApples();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Map::seeEnemies()
{
    for (auto& enemy : Enemies)
    {
        MoveEnemy(enemy);
    }
}

void Map::generateMap()
{
    //Map 
    space = new string *[SIZE_X];
    for (int i = 0; i < SIZE_X; i++) {
        space[i] = new string[SIZE_Y];
        for (int j = 0; j < SIZE_Y; j++) {
            space[i][j] = " ";
        }
    }

    for (int i = 0; i < 13 ;i++)
    {
        for (int j = 0; j < 13; j++)
        {
            space[i][j] = GRASS;
            space[i+17][j] = GRASS;
        }
        
    }
    for (int i = 0; i < 5; i++)
    {
        if(i < 3)
        {  
            space[i][13] = GRASS;
            space[29-i][13] = GRASS;
        }
        space[i][14] = GRASS;
        space[29-i][14] = GRASS;
    }
        
    for (int i = 1; i < 15; i++)
    { 
        if(i < 13)
        {
            space[29-i][5] = " ";
            space[29-i][0] = " ";
        }
        space[i][1] = " ";
    }
    for(int i = 0; i < 11; i++)
    {
        if(i < 8)
        {
            space[1][i+2] = " ";
            space[2][i+2] = " ";
        }
        space[27][i] = " ";
        space[28][i] = " ";
    }

    for (int i = 0; i < 7; i++)
    {
        if(i < 3)
        {
            space[21][9+i] = " ";
            space[20][9+i] = " ";
        }
        space[28-i][9] = " ";
        space[2+i][10] = " ";
    }
}

void Map::generateCherryApple()
{
    for (int i = 0; i < 8; i++) {
        int x, y;
        do {
            x = std::rand() % 30;
            y = std::rand() % 12;
        } while (space[x][y] != GRASS);
        cherries[i] = Cherry(x, y);
        space[cherries[i].getX()][cherries[i].getY()] = cherries[i].getBody();

        do {
            x = std::rand() % 30;
            y = std::rand() % 12;
        } while (space[x][y] != GRASS);
        apples[i] = Apple(x, y);
        space[apples[i].getX()][apples[i].getY()] = apples[i].Body();
    }
    
}

void Map::Shoot()
{
    if (proyectile.isTimerReady())
{
    int projX = proyectile.getX();
    int projY = proyectile.getY();

    if (space[projX][projY] == "\x6")
    {
        space[projX][projY] = " ";
        con->setCursor(projY, projX);
        *con << " ";
    }

    int newX = character.getX();
    int newY = character.getY();

    switch (character.getDirection())
    {
        case Direction::Left:
            newX -= 1;
            break;
        case Direction::Right:
            newX += 1;
            break;
        case Direction::Up:
            newY -= 1;
            break;
        case Direction::Down:
            newY += 1;
            break;
    }

    if (newX >= 0 && newX < SIZE_X && newY >= 0 && newY < SIZE_Y)
    {
        proyectile = Proyectile(newX, newY, character.getDirection());
        proyectile.setActive();
        MoveProyectile();
    }
}

}

void Map::toString()
{
    con->setCursor(0,0);
    for (int i = 0; i < SIZE_X; i++)
    {
        for (int j = 0; j < SIZE_Y; j++)
        {
            con->setCursor(j, i);
            *con << space[i][j];
        }
    }
    
    printData();
}

void Map::printData()
{
    con->setCursor(0,35);
    *con << "Level: #" << std::to_string(level);
    con->setCursor(1,35);
    *con << "Lifes: " << std::to_string(lifes);
    con->setCursor(2,35);
    *con << "Score: " << std::to_string(score);
    
}

void Map::restartGame()
{
    lifes--;   
    if(lifes > 0)
    {
        running = false;
        //Eliminar enemigos o vaciarlos
        for (int i = 0; i < Enemies.size(); i++)
        {
            con->setCursor(Enemies[i].getY(),Enemies[i].getX());
            space[Enemies[i].getX()][Enemies[i].getY()] = " ";
            *con << " ";
        }
        Enemies.clear();
        //Devolver al jugador al punto de inicio
        con->setCursor(character.getY(),character.getX());                
        space[character.getX()][character.getY()] = " ";
        *con << space[character.getX()][character.getY()];
        character.setX(Character_SPAWN_X);
        character.setY(Character_SPAWN_Y);
        space[character.getX()][character.getY()] = CHARACTER_BODY;
        con->setCursor(character.getY(),character.getX());
        *con << space[character.getX()][character.getY()];
        printData();
        running = true;
    }
    else
    {
        gameOver = true;
    }

}

void Map::MoveCharacter(Direction dir)
{   
    if(character.getDirection() != dir)
    {
        character.setDirection(dir);
    }else
    {
        if(character.getX() == 0 && character.getDirection() == Direction::Left
            || character.getX() == SIZE_X-1 && character.getDirection() == Direction::Right
            || character.getY() == 0 && character.getDirection() ==Direction::Up 
            || character.getY() == SIZE_Y-1 && character.getDirection() == Direction::Down)
        {}
        else
        {
            int x = character.getX();
            int y = character.getY();
            switch(dir)
            {
                case Direction::Left:
                    character.setX(character.getX() - 1);
                break;
                case Direction::Right:
                    character.setX(character.getX() + 1);
                break;
                case Direction::Up:
                    character.setY(character.getY() - 1);
                break;
                case Direction::Down:
                    character.setY(character.getY() + 1);
                break;
            }

            if(!character.isUntouchable(space[character.getX()][character.getY()]))
            {
                con->setCursor(y,x);
                space[x][y] = " ";
                *con << space[x][y];
                space[character.getX()][character.getY()] = CHARACTER_BODY;
                con->setCursor(character.getY(),character.getX());
                *con << space[character.getX()][character.getY()];
            }else
            {
                character.setX(x);
                character.setY(y);
            }

            for (int  i = 0; i < 8; i++)
            {
                if(cherries[i].compareCoords(character.getX(), character.getY()))
                {
                    score = cherries[i].ObtainCherry(score);
                    printData();
                }
            }

        }
    }
}

void Map::SeeApples()
{
    for (int i = 0; i < 8; i++)
    {
        if(apples[i].getY() < SIZE_Y-1 && space[apples[i].getX()][apples[i].getY() + 1] == " ")
        {
            
            if(apples[i].isActive() && apples[i].isFalling())
            {
                //Se cae
                MoveApple(apples[i]);
            }else 
            {
                apples[i].setFall();
            }
        }else if(apples[i].isActive() && apples[i].isFalling())
        {
            if(apples[i].getY() <= SIZE_Y-2)
            {
                if(space[apples[i].getX()][apples[i].getY()+1] == CHARACTER_BODY)
                {
                    restartGame();
                }else
                {
                    apples[i].setOff();
                }
            }
            space[apples[i].getX()][apples[i].getY()] = " ";
            con->setCursor(apples[i].getY(),apples[i].getX());
            *con << space[apples[i].getX()][apples[i].getY()];
            apples[i].setOff();
        }
    }
}

void Map::MoveApple(Apple &app)
{
    space[app.getX()][app.getY()] = " ";
    con->setCursor(app.getY(),app.getX());
    *con << space[app.getX()][app.getY()];
    app.setY(app.getY() + 1);
    space[app.getX()][app.getY()] = app.Body();
    con->setCursor(app.getY(),app.getX());
    *con << space[app.getX()][app.getY()];
}

void Map::MoveEnemy(Enemy & em)
{
    int x = em.getX();
    int y = em.getY();
    Direction dir = em.getDirection();

    if ((x <= 0 && dir == Direction::Left) ||
        (x >= SIZE_X - 1 && dir == Direction::Right) ||
        (y <= 0 && dir == Direction::Up) ||
        (y >= SIZE_Y - 1 && dir == Direction::Down))
    {
        em.changeDirection();
    }
    else
    {
        int newX = x;
        int newY = y;

        switch (dir)
        {
            case Direction::Left:  newX--; break;
            case Direction::Right: newX++; break;
            case Direction::Up:    newY--; break;
            case Direction::Down:  newY++; break;
        }

        if (em.isThere(character.getX(), character.getY()))
        {
            restartGame();
        }else if(em.isThere(proyectile.getX(),proyectile.getY()))
        {
            int index = searchEnemyIndex(proyectile.getX(), proyectile.getY());
            Enemies.erase(Enemies.begin() + index);
            proyectile.setActive();
            score += POINTS_KILL;
            space[proyectile.getX()][proyectile.getY()] = " ";
            con->setCursor(proyectile.getY(),proyectile.getX());
            *con << space[proyectile.getX()][proyectile.getY()];
            printData();
            return;
        }
        else if (em.isUntouchable(space[newX][newY]))
        {
            em.changeDirection();
        }
        else
        {
            con->setCursor(y, x);
            space[x][y] = " ";
            *con << space[x][y];

            em.setX(newX);
            em.setY(newY);

            space[newX][newY] = ENEMY_BODY;
            con->setCursor(newY, newX);
            *con << space[newX][newY];
        }
    }
}
