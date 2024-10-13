#include"infantry_regiment.hpp"
class Brigade:public Unit{
public:
    Regiment*(*regiments)[];
    unsigned short regs;
    Sprite thouDigit;
    Sprite hunDigit;
    Sprite tenDigit;
    Sprite oneDigit;
    Brigade(
            float,
            float,
            float,
            float,
            bool,
            Regiment*(*)[],
            unsigned short,
            bool*,
            Regiment*(*)[],
            unsigned short,
            Hill(*)[],
            unsigned short);
private:
    bool orderedUnits;
    void brigadeSendOrder(OrderType,float,float);
    void brigadeUpdateState(Unit*(*)[],unsigned short);
    float brigadeGetZ(float,float);
};
