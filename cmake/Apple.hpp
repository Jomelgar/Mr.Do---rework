
#include<string>

using std::string;

class Apple
{
public:
    Apple() : X(0), Y(0){}
    Apple(int x, int y) : X(x), Y(y) {}

    void setOn(){active = true;}
    void setOff(){active = false;}
    void setFall(){fall = true;}
    void setY(int y){Y = y;}

    int getX(){return X;}
    int getY(){return Y;}
    bool isActive() {return active;}
    bool isFalling() {return fall;}
    string Body(){return body;}
private:
    int X, Y;
    bool active = true, fall = false;
    string body = "\x10";
};