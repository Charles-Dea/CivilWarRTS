#include<iostream>
#include<math.h>
#include"hill.hpp"
using namespace std;
Pair Hill::getXs(float yInput,float zInput){
    float adjustedY=ySteepness*yInput-ySteepness*y;
    double leftSide=zInput+adjustedY*adjustedY-height;
    if(leftSide<0.0)
        leftSide=-leftSide;
    float root=sqrt(leftSide);
    Pair vals;
    if(root>0.0f){
        vals.big=root;
        vals.small=-root;
    }else{
        vals.big=-root;
        vals.small=root;
    }
    float xOffset=xSteepness*x;
    vals.big+=xOffset;
    vals.small+=xOffset;
    vals.big/=xSteepness;
    vals.small/=xSteepness;
    return vals;
}
Pair Hill::getYs(float xInput,float zInput){
    float adjustedX=xSteepness*xInput-xSteepness*x;
    double leftSide=zInput+adjustedX*adjustedX-height;
    if(leftSide<0.0)
        leftSide=-leftSide;
    float root=sqrt(leftSide);
    Pair vals;
    if(root>0.0f){
        vals.big=root;
        vals.small=-root;
    }else{
        vals.big=-root;
        vals.small=root;
    }
    float yOffset=ySteepness*y;
    vals.big+=yOffset;
    vals.small+=yOffset;
    vals.big/=ySteepness;
    vals.small/=ySteepness;
    return vals;
}
float Hill::getZ(float xInput,float yInput){
    float radRotation=rotation/180.0f*M_PI*-1.0f;
    float sinRotation=sin(radRotation);
    float cosRotation=cos(radRotation);
    float adjustedX=xSteepness*((xInput-x)*cosRotation-(yInput-y)*sinRotation+x)-xSteepness*x;
    float adjustedY=ySteepness*((yInput-y)*cosRotation+(xInput-x)*sinRotation+y)-ySteepness*y;
    return -(adjustedX*adjustedX)-(adjustedY*adjustedY)+height;
}
float hillGetZ(Hill(*hills)[],unsigned short numHills,float x,float y){
    float z=0.0f;
    for(unsigned short count=0;count<numHills;++count){
        float hillZ=(*hills)[count].getZ(x,y);
        if(z<hillZ)
            z=hillZ;
    }
    return z;
}
Hill::Hill(float xPos,float yPos,float h,float xs,float ys,float r){
    x=xPos;
    y=yPos;
    height=h;
    xSteepness=xs;
    ySteepness=ys;
    rotation=r;
}
