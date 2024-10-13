#include"brigade.hpp"
class Textbox{
public:
    char(*text)[];
    float x;
    float y;
    float length;
    float height;
    Sprite box;
    Sprite(*chars)[];
    unsigned short setChars;
    unsigned short cells;
    void(*func)(char*);
    void displayString();
    Textbox(float,float,short,void(*)(char*));
};
