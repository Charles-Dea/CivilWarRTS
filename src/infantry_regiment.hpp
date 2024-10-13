#include"cavalry_regiment.hpp"
class InfantryRegiment:public Regiment{
public:
	Sprite hunDigit;
	Sprite tenDigit;
	Sprite oneDigit;
    InfantryRegiment(
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
    void infUpdateState(Unit*(*)[],unsigned short);
    //bool infCanFire(Unit*);
};
