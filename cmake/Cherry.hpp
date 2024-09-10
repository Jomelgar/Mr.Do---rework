#pragma once

#include <string>

using std::string;

string BODY_CHERRY = "\x09";
int POINTS = 300;

class Cherry
{
    public:
    Cherry(int x, int y) : X(x), Y(y) {}
    Cherry() : X(0), Y(0) {}

    int ObtainCherry(int score)
    {
        if(!obtain)
        {
            obtain = true;
            return score + POINTS;
        }
        return score + 0;
    }

    bool compareCoords(int x, int y)
    {
        return x == X && y == Y;
    }

    int getX(){return X;}
    int getY(){return Y;}
    string getBody(){return body;}
    Cherry & operator=(const Cherry& other) = default;
    bool isObtain(){return obtain;}
    private:
    int X, Y;
    string body = BODY_CHERRY;
    bool obtain = false;
};