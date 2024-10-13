#include<iostream>
#include<math.h>
#include"renderer.hpp"
void Brigade::brigadeSendOrder(OrderType order,float x,float y){
    if(order==halt){
        hasOrder=false;
        for(unsigned short count=0;count<regs;++count){
            Regiment*reg=(*regiments)[count];
            if(reg)
                (*reg.*reg->sendOrder)(halt,NULL,NULL);
        }
    }else{
        hasOrder=true;
        orderType=order;
        destinationX=x;
        destinationY=y;
        orderedUnits=false;
    }
}
void Brigade::brigadeUpdateState(Unit*(*enemies)[],unsigned short){
    if(rotation>=MAX_DEGREES)
        rotation-=MAX_DEGREES;
    if(rotation<0)
        rotation+=MAX_DEGREES;
    if(hasOrder){
        if(orderType==march){
            float dx=destinationX;
            float dy=destinationY;
            if(!orderedUnits){
                float x=xPosition;
                float y=yPosition;
                float temp;
                float radRotation=findRelativeAngle(dx,dy,&temp)/180.0f*M_PI;
                for(unsigned short count=0;count<regs;++count){
                    Regiment*reg=(*regiments)[count];
                    if(reg){
                        float regX=reg->xPosition;
                        float regY=reg->yPosition;
                        float rotatedX=(regX-x)*cos(radRotation)-(regY-y)*sin(radRotation)+x;
                        float rotatedY=(regY-y)*cos(radRotation)+(regX-x)*sin(radRotation)+y;
                        float xOffset=rotatedX-x+dx;
                        float yOffset=rotatedY-y+dy;
                        (*reg.*reg->sendOrder)(march,xOffset,yOffset);
                        reg->orderedRotate=false;
                    }
                }
                rotation=temp;
                orderedUnits=true;
            }else{
                unsigned short numRegs=regs;
                for(unsigned short count=0;count<numRegs;++count){
                    Regiment*reg=(*regiments)[count];
                    if(reg){
                        if(!reg->hasOrder&&!reg->orderedRotate){
                            float radRotation=rotation/180.0f*M_PI+M_PI;//for some reason the code is 180 degrees off
                            double sinRotation=sin(radRotation);
                            double cosRotation=cos(radRotation);
                            (*reg.*reg->sendOrder)(rotate,0-sinRotation+reg->xPosition,cosRotation+reg->yPosition);
                        }
                    }
                }
            }
            marchTowards(dx,dy);
        }else if(orderType==commanderMarch){
            marchTowards(destinationX,destinationY);
        }
    }
    if(xPosition==destinationX&&yPosition==destinationY&&hasOrder){
        bool unitsRotated=false;
        for(unsigned short count=0;count<regs&&!unitsRotated;++count){
            Regiment*reg=(*regiments)[count];
            if(reg){
                unitsRotated=reg->rotated;
            }
        }
        hasOrder=!unitsRotated;
    }
    if(cs){
        if(selected){
            button->sprite.texture=CS_BRIG_TEXTURE_SEL;
        }else{
            button->sprite.texture=CS_BRIG_TEXTURE_UNSEL;
        }
    }else{
        if(selected){
            button->sprite.texture=US_BRIG_TEXTURE_SEL;
        }else{
            button->sprite.texture=US_BRIG_TEXTURE_UNSEL;
        }
    }
    unsigned short brigadeSize=0;
    for(unsigned short count=0;count<regs;++count){
        if((*regiments)[count])
            brigadeSize+=(*regiments)[count]->size;
    }
    button->x=xPosition;
    button->y=yPosition;
    button->sprite.x=xPosition;
    button->sprite.y=yPosition;
    thouDigit.x=xPosition+ICON_LENGTH/2+NUMBER_LENGTH/2+NUMBER_OFFSET;
    hunDigit.x=thouDigit.x+NUMBER_LENGTH+NUMBER_OFFSET;
    tenDigit.x=hunDigit.x+NUMBER_LENGTH+NUMBER_OFFSET;
    oneDigit.x=tenDigit.x+NUMBER_LENGTH+NUMBER_OFFSET;
    float y=yPosition;
    thouDigit.y=y;
    hunDigit.y=y;
    tenDigit.y=y;
    oneDigit.y=y;
    unsigned short thouDigitNum=brigadeSize/1000;
    unsigned short hunDigitNum=(brigadeSize-thouDigitNum*1000)/100;
    unsigned short tenDigitNum=(brigadeSize-thouDigitNum*1000-hunDigitNum*100)/10;
    unsigned short oneDigitNum=brigadeSize-thouDigitNum*1000-hunDigitNum*100-tenDigitNum*10;
    thouDigit.texture=getNumSprite(thouDigitNum);
    hunDigit.texture=getNumSprite(hunDigitNum);
    tenDigit.texture=getNumSprite(tenDigitNum);
    oneDigit.texture=getNumSprite(oneDigitNum);
    float totalSpeed=0.0f;
    unsigned short livingRegs=0;
    for(unsigned short count=0;count<regs;++count){
        if((*regiments)[count]){
            totalSpeed+=(*regiments)[count]->speed;
            ++livingRegs;
        }
    }
    speed=totalSpeed/livingRegs;
}
float Brigade::brigadeGetZ(float x,float y){
    return hillGetZ(hills,numHills,x,y)+0.2f;
}
Brigade::Brigade(
        float x,
        float y,
        float r,
        float s,
        bool isCS,
        Regiment*(*units)[],
        unsigned short numUnits,
        bool*pcs,
        Regiment*(*reg)[],
        unsigned short nr,
        Hill(*hillArr)[],
        unsigned short nh){
    xPosition=x;
    yPosition=y;
    rotation=r;
    size=s;
    initialSize=s;
    cs=isCS;
    regiments=units;
    regs=numUnits;
    for(unsigned short count=0;count<numUnits;++count){
        (*units)[count]->brigade=this;
    }
    Texture texture;
    if(isCS){
        texture=CS_BRIG_TEXTURE_UNSEL;
    }else{
        texture=US_BRIG_TEXTURE_UNSEL;
    }
    button=new Button(
        x,
        y,
        0.5f,
        ICON_LENGTH,
        ICON_WIDTH,
        texture,
        (void*)&changeSelected,
        (void*)&deselect,
        (void*)this
    );
    selected=false;
    hasOrder=false;
    thouDigit.x=x+ICON_LENGTH/2+NUMBER_LENGTH/2+NUMBER_OFFSET;
    hunDigit.x=thouDigit.x+NUMBER_LENGTH+NUMBER_OFFSET;
    tenDigit.x=hunDigit.x+NUMBER_LENGTH+NUMBER_OFFSET;
    oneDigit.x=tenDigit.x+NUMBER_LENGTH+NUMBER_OFFSET;
    thouDigit.y=y;
    hunDigit.y=y;
    tenDigit.y=y;
    oneDigit.y=y;
    thouDigit.z=0.45f;
    hunDigit.z=0.45f;
    tenDigit.z=0.45f;
    oneDigit.z=0.45f;
    thouDigit.l=NUMBER_LENGTH;
    hunDigit.l=NUMBER_LENGTH;
    tenDigit.l=NUMBER_LENGTH;
    oneDigit.l=NUMBER_LENGTH;
    thouDigit.w=ICON_WIDTH;
    hunDigit.w=ICON_WIDTH;
    tenDigit.w=ICON_WIDTH;
    oneDigit.w=ICON_WIDTH;
    thouDigit.heapAllocated=false;
    hunDigit.heapAllocated=false;
    tenDigit.heapAllocated=false;
    oneDigit.heapAllocated=false;
    thouDigit.visible=true;
    hunDigit.visible=true;
    tenDigit.visible=true;
    oneDigit.visible=true;
    destinationX=x;
    destinationY=y;
    updateState=(void(Unit::*)(Unit*(*)[],unsigned short))&Brigade::brigadeUpdateState;
    sendOrder=(void(Unit::*)(OrderType,float,float))&Brigade::brigadeSendOrder;
    type=brigade;
    playerCs=pcs;
    canSee=&Unit::unitCanSee;
    allRegiments=(Unit*(*)[])reg;
    numRegiments=nr;
    hills=hillArr;
    numHills=nh;
    getZ=(float(Unit::*)(float,float))&Brigade::brigadeGetZ;
}
