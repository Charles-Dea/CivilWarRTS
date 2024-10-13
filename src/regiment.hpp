#include"unit.hpp"
#include"audio.hpp"
#define FOOT_SPEED 0.00125
class Regiment:public Unit{
public:
    float length;
    bool rotated;
    Unit*brigade;
    bool orderedRotate;
    bool visible;
    double reload;
    bool firing;
    float range;
    float rotationSpeed;
    unsigned int fireSound;
    double neededReload;
    double baseReload;
    bool(Regiment::*canFire)(Unit*);
    bool regCanFire(Unit*);
    bool regCanSee(Unit*);
    void regSendOrder(OrderType,float,float);
    float regGetZ(float,float);
    void processOrder();
    void fire(Unit*(*)[],unsigned short,float);
    double getReload();
};
