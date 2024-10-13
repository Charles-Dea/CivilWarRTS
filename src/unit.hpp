#include"button.hpp"
#include"hill.hpp"
#define ICON_LENGTH 0.05f
#define ICON_WIDTH 0.025f
#define NUMBER_LENGTH 0.0125f
#define NUMBER_OFFSET 0.01f
#define MAX_DEGREES 360
enum OrderType{
    march,
    rotate,
    commanderMarch,
    halt
};
enum UnitType{
    infantry_regiment,
    artillery_battery,
    cavalry_regiment,
    brigade
};
struct Point{
    float x;
    float y;
};
struct Line{
    Point p1;
    Point p2;
};
class AbiManipulator{};
class Unit{
public:
    float xPosition;
    float yPosition;
    float rotation;
    float size;
    bool cs;
    bool selected;
    Button*button;
    bool hasOrder;
    float destinationX;
    float destinationY;
    OrderType orderType;
    bool*playerCs;
    float initialSize;
    bool sorted;
    double distance;
    UnitType type;
    Unit*(*allRegiments)[];
    unsigned short numRegiments;
    Hill(*hills)[];
    unsigned short numHills;
    float speed;
    void(Unit::*sendOrder)(OrderType,float,float);
    void(Unit::*updateState)(Unit*(*)[],unsigned short);
    bool(Unit::*canSee)(Unit*);
    float(Unit::*getZ)(float,float);
    Point rotatePoint(Point);
    Point rotatePoint(Point,float);
    bool unitCanSee(Unit*);
    bool linesIntersect(Line,Line);
    bool intersect(Line,Line,AbiManipulator*,float(AbiManipulator::*)(float,float),Unit*);
protected:
    unsigned int getNumSprite(unsigned short);
    float findRelativeAngle(float,float,float*);//returns angle in degrees
    double findDistance(float,float,float*,float*);
    void marchTowards(float,float);
};
void changeSelected(Unit*);
void deselect(Unit*);
