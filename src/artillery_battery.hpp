#include"regiment.hpp"
class ArtilleryBattery:public Regiment{
public:
    Sprite gunDigit;
    Sprite limberedSprite;
    bool limbered;
    ArtilleryBattery(float,float,float,unsigned short,bool,bool*,Regiment*(*)[],unsigned short,Hill(*)[],unsigned short);
private:
    unsigned short guns;
    void artUpdateState(Unit*(*)[],unsigned short);
    bool artCanFire(Unit*);
};
