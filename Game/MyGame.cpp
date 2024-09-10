#include "MyGame.h"
#include "Map.hpp"
#include <thread>
#include <iostream>

#define UPGRADE_LEVEL 2400

void MyGame::run()
{
    int update_score = 0;
    con.setForecolor(CColor::Grey);
    con.setBackcolor(CColor::Black);
    con.clearScreen();
    con.setCursor(10, 10);
    bool menu = false;
    std::string name = "User1";
    Map* map = nullptr;
    con.flushKeybuffer();
    while (con.isActive()) {
        uint32_t key = con.getKey();

        if(map)
        {
            if (key != SDLK_UNKNOWN) {
                switch (key) {
                    case SDLK_LEFT:
                        map->MoveCharacter(Direction::Left);
                    break;
                    case SDLK_RIGHT:
                        map->MoveCharacter(Direction::Right);
                    break;
                    case SDLK_UP:
                        map->MoveCharacter(Direction::Up);
                    break;
                    case SDLK_DOWN:
                        map->MoveCharacter(Direction::Down);
                    break;
                    case SDLK_SPACE:
                        map->Shoot();
                    break;
                }
                con.refresh();
            }
        
            if(map->isGameOver())
            {
                delete map;
                map = nullptr;
                con.clearScreen();
                menu = false;
            }else if(update_score + UPGRADE_LEVEL <= map->getScore())
            {
                update_score += map->getScore();
                int level = map->getLevel() + 1;
                int lifes = map->getLifes();
                delete map;
                map = new Map(&con,update_score,lifes,level);
            }
        }else
        {
            if(!menu){
                con.setForecolor(CColor::White);
                con.setCursor(1, 10);
                con << "MENU";
                con.setCursor(3,10);
                con << "- Jugar";
                con.setCursor(4,10);
                con << "PRESIONE CUALQUIER TECLA";
                menu = true;
            }
             if (key != SDLK_UNKNOWN) {
                    con.clearScreen();
                    con.setForecolor(CColor::PaleGreen3);
                    update_score = 0;
                    map = new Map(&con);
                con.refresh();
            }
        }
    }
}
