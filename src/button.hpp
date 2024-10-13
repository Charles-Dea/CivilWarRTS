#include"sprite.hpp"
class Button{
public:
    float x;
    float y;
	float z;
    float length;
    float width;
	Sprite sprite;
	void performAction();
	void performInvAction();
    Button(float,float,float,float,float,unsigned int,void*,void*,void*);
private:
    void*func;
	void*invFunc;
    void*parent;
};
