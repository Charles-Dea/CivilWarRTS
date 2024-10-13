#include<GLFW/glfw3.h>
#include"artillery_battery.hpp"
class CavalryRegiment:public Regiment{
public:
	Sprite hunDigit;
	Sprite tenDigit;
	Sprite oneDigit;
    Sprite mountedSprite;
    bool mounted;
    CavalryRegiment(
            float,
            float,
            float,
            float,
            bool,
            bool*,
            Regiment*(*)[],
            unsigned short,
            Hill(*)[],
            unsigned short);
private:
    void cavUpdateState(Unit*(*)[],unsigned short);
    bool cavCanFire(Unit*);
};
