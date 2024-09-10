#ifndef __GAME_H__
#define __GAME_H__
#include "TextConsole.h"

class MyGame: public CClient {
public:
    MyGame(TextConsole& con): CClient(con) {}

    void run() override;
};

#endif