#include<iostream>
#include<math.h>
#include"renderer.hpp"
#define SPEED 0.00125f
using namespace std;
enum Orientation{
    collinear,
    clockwise,
    counterclockwise
};
Orientation findOrientation(Point p1,Point p2,Point p3){
    float val=(p2.y-p1.y)*(p3.x-p2.x)-(p2.x-p1.x)*(p3.y-p2.y);
    if(!val)
        return collinear;
    if(val>0)
        return clockwise;
    return counterclockwise;
}
float max(float x,float y){
    if(x>=y)
        return x;
    return y;
}
float min(float x,float y){
    if(x<=y)
        return x;
    return y;
}
bool onSegment(Line line,Point point){
    bool intersectsX=point.x<=max(line.p1.x,line.p2.x)&&point.x>=min(line.p1.x,line.p2.x);
    bool intersectsY=point.y<=max(line.p1.y,line.p2.y)&&point.y>=min(line.p1.y,line.p2.y);
    return intersectsX&&intersectsY;
}
void changeSelected(Unit*unit){
    if(unit->cs==*unit->playerCs){
        unit->selected=!unit->selected;
    }
}
void deselect(Unit*unit){
    unit->selected=false;
}
float getLineData(float*yIntercept,Line line){
    Line orderedLine;
    if(line.p1.x<=line.p2.x){
        orderedLine=line;
    }else{
        orderedLine.p1=line.p2;
        orderedLine.p2=line.p1;
    }
    float xDiff=line.p2.x-line.p1.x;
    float slope;
    if(xDiff){
        slope=(line.p2.y-line.p1.y)/xDiff;
        *yIntercept=line.p1.y-line.p1.x*slope;
    }else{
        slope=0.0f;
        *yIntercept=sqrt(-1.0f);//set yIntercept to nan
    }
    return slope;
}
bool Unit::linesIntersect(Line fireLine,Line objectLine){
    Orientation o1=findOrientation(fireLine.p1,fireLine.p2,objectLine.p1);
    Orientation o2=findOrientation(fireLine.p1,fireLine.p2,objectLine.p2);
    Orientation o3=findOrientation(objectLine.p1,objectLine.p2,fireLine.p1);
    Orientation o4=findOrientation(objectLine.p1,objectLine.p2,fireLine.p2);
    bool cond1=o1!=o2&&o3!=o4;
    bool cond2=o1==collinear&&onSegment(fireLine,objectLine.p1);
    bool cond3=o2==collinear&&onSegment(fireLine,objectLine.p2);
    bool cond4=o3==collinear&&onSegment(objectLine,fireLine.p1);
    bool cond5=o4==collinear&&onSegment(objectLine,fireLine.p2);
    return cond1||cond2||cond3||cond4||cond5;
}
bool Unit::intersect(
        Line fireLine,
        Line objectLine,
        AbiManipulator*object,
        float(AbiManipulator::*getZ)(float,float),
        Unit*target){
    if(!linesIntersect(fireLine,objectLine))
        return false;
    float a1=fireLine.p2.y-fireLine.p1.y;
    float b1=fireLine.p1.x-fireLine.p2.x;
    float c1=a1*fireLine.p1.x+b1*fireLine.p1.y;
    float a2=objectLine.p2.y-objectLine.p1.y;
    float b2=objectLine.p1.x-objectLine.p2.x;
    float c2=a2*objectLine.p1.x+b2*objectLine.p1.y;
    float det=a1*b2-a2*b1;
    float x=(b2*c1-b1*c2)/det;
    float y=(a1*c2-a2*c1)/det;
    Line zLine;
    float controlVar;
    if(fireLine.p1.x==fireLine.p2.x){
        zLine={
            {fireLine.p1.y,(*this.*this->getZ)(fireLine.p1.x,fireLine.p1.y)},
            {fireLine.p2.y,(*target.*target->getZ)(fireLine.p2.x,fireLine.p2.y)}};
        controlVar=y;
    }else{
        zLine={
            {fireLine.p1.x,(*this.*this->getZ)(fireLine.p1.x,fireLine.p1.y)},
            {fireLine.p2.x,(*target.*target->getZ)(fireLine.p2.x,fireLine.p2.y)}};
        controlVar=x;
    }
    float zIntercept;
    float zSlope=getLineData(&zIntercept,zLine);
    return (zSlope*controlVar+zIntercept)<=(*object.*getZ)(x,y);
}
unsigned int Unit::getNumSprite(unsigned short num){
    if(num>9){
        cout<<num<<" passed to getNumSprite\n";
        return NULL_TEXTURE;
    }
    return ZERO+num;
}
float Unit::findRelativeAngle(float x,float y,float*angle){
    float a=atan2(y-yPosition,x-xPosition)*180.0/M_PI+90.0;
    if(angle)
        *angle=a;
    a-=rotation;
    return a;
}
double Unit::findDistance(float x,float y,float*xd,float*yd){
    float xDistance=x-xPosition;
    float yDistance=y-yPosition;
    if(xd)
        *xd=xDistance;
    if(yd)
        *yd=yDistance;
    return sqrt(xDistance*xDistance+yDistance*yDistance);
}
void Unit::marchTowards(float x,float y){
    float xDistance;
    float yDistance;
    float distance=findDistance(x,y,&xDistance,&yDistance);
    float time=distance/speed;
    float xSpeed=xDistance/time;
    float ySpeed=yDistance/time;
    if(xSpeed<0.0f)
        xSpeed*=-1.0f;
    if(ySpeed<0.0f)
        ySpeed*=-1.0f;
    float newX=xPosition;
    float newY=yPosition;
    if(x>=xPosition+xSpeed){
        newX+=xSpeed;
    }else if(x<=xPosition-xSpeed){
        newX-=xSpeed;
    }else{
        newX=x;
    }
    if(y>=yPosition+ySpeed){
        newY+=ySpeed;
    }else if(y<=yPosition-ySpeed){
        newY-=ySpeed;
    }else{
        newY=y;
    }
    float heightDiff=(hillGetZ(hills,numHills,xPosition,yPosition)-hillGetZ(hills,numHills,newX,newY))/10.0f+1.0f;
    if(heightDiff<0.0f)
        printf("WARNING: heightDiff is negative\n");
    xPosition=newX*heightDiff;
    yPosition=newY*heightDiff;
}
Point Unit::rotatePoint(Point point,float rot){
    double radRotation=rot/180.0f*M_PI;
    double sinRotation=sin(radRotation);
    double cosRotation=cos(radRotation);
    float x=xPosition;
    float y=yPosition;
    Point outPoint;
    outPoint.x=(point.x-x)*cosRotation-(point.y-y)*sinRotation+x;
    outPoint.y=(point.y-y)*cosRotation+(point.x-x)*sinRotation+y;
    return outPoint;
}
Point Unit::rotatePoint(Point point){
    return rotatePoint(point,rotation);
}
bool Unit::unitCanSee(Unit*target){
    Line fireLine={{xPosition,yPosition},{target->xPosition,target->yPosition}};
    for(unsigned short count=0;count<numHills;++count){
        float hillY=(*hills)[count].y;
        Pair xVals=(*hills)[count].getXs(hillY,0.0f);
        float hillRotation=(*hills)[count].rotation;
        Line xLine={rotatePoint({xVals.big,hillY},hillRotation),rotatePoint({xVals.small,hillY},hillRotation)};
        AbiManipulator*object=(AbiManipulator*)&(*hills)[count];
        float(AbiManipulator::*getZ)(float,float)=(float(AbiManipulator::*)(float,float))&Hill::getZ;
        if(intersect(fireLine,xLine,object,getZ,target))
            return false;
        float hillX=(*hills)[count].x;
        Pair yVals=(*hills)[count].getYs(hillX,0.0f);
        Line yLine={rotatePoint({xVals.big,hillY},hillRotation),rotatePoint({xVals.small,hillY},hillRotation)};
        if(intersect(fireLine,yLine,object,getZ,target))
            return false;
    }
    for(unsigned short count=0;count<numRegiments;++count){
        Regiment*reg=(Regiment*)(*allRegiments)[count];
        if(reg&&reg!=target&&reg!=this){
            float regX=reg->xPosition;
            float regY=reg->yPosition;
            float regL=reg->length/2.0f;
            Line regLine={reg->rotatePoint({regX-regL,regY}),reg->rotatePoint({regX+regL,regY})};
            if(intersect(fireLine,regLine,(AbiManipulator*)reg,(float(AbiManipulator::*)(float,float))reg->getZ,target))
                return false;
        }
    }
    return true;
}
