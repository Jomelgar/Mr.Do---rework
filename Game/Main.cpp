#include <iostream>
#include "TextConsole.h"
#include "MyGame.h"

int main(int argc, char *argv[])
{
    TextConsole con(80, 24);

    if (!con.activate()) {
        return 1;
    }
    MyGame game(con);

    con.run(game);

    return 0;
}