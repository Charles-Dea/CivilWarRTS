#include<iostream>
#include<math.h>
#include"renderer.hpp"
#define DAMAGE 10
using namespace std;
void ArtilleryBattery::artUpdateState(Unit*(*enemies)[],unsigned short enemyCount){
    if(rotation>=MAX_DEGREES)
        rotation-=MAX_DEGREES;
    if(rotation<0)
        rotation+=MAX_DEGREES;
    if(hasOrder){
        processOrder();
    }else{
        if(reload>=neededReload/guns){
            fire(enemies,enemyCount,4.58333f);
        }else{
            firing=false;
            for(unsigned short count=0;count<enemyCount;++count){
                if((*enemies)[count]){
                    if(artCanFire((*enemies)[count])){
                        firing=true;
                    }
                }
            }
            reload+=0.25;
        }
    }
    if((orderType==march&&destinationX==xPosition&&destinationY==yPosition)||(orderType==rotate&&rotated))
        hasOrder=false;
    button->x=xPosition;
    button->y=yPosition;
    button->sprite.x=xPosition;
    button->sprite.y=yPosition;
    bool brigadeSelected;
    if(brigade){
        brigadeSelected=brigade->selected;
    }else{
        brigadeSelected=false;
    }
    if(selected||brigadeSelected){
        if(firing){
            button->sprite.texture=FIRING_ART_TEXTURE_SEL;
        }else if(cs){
            button->sprite.texture=CS_ART_TEXTURE_SEL;
        }else{
            button->sprite.texture=US_ART_TEXTURE_SEL;
        }
    }else{
        if(firing){
            button->sprite.texture=FIRING_ART_TEXTURE_UNSEL;
        }else if(cs){
            button->sprite.texture=CS_ART_TEXTURE_UNSEL;
        }else{
            button->sprite.texture=US_ART_TEXTURE_UNSEL;
        }
    }
    gunDigit.x=xPosition+ICON_LENGTH/2+NUMBER_LENGTH/2+NUMBER_OFFSET;
    gunDigit.y=yPosition;
    gunDigit.texture=getNumSprite(guns);
    limberedSprite.x=xPosition-ICON_LENGTH/2-NUMBER_LENGTH/2-NUMBER_OFFSET;
    limberedSprite.y=yPosition;
    limberedSprite.texture=limbered?Y:N;
    speed=limbered?FOOT_SPEED:FOOT_SPEED/2;
}
bool ArtilleryBattery::artCanFire(Unit*target){
    return limbered?false:regCanFire(target);
}
ArtilleryBattery::ArtilleryBattery(
        float x,
        float y,
        float r,
        unsigned short g,
        bool isCS,
        bool*pCs,
        Regiment*(*regs)[],
        unsigned short numRegs,
        Hill(*hillArr)[],
        unsigned short nh
        ){
    xPosition=x;
    yPosition=y;
    rotation=r;
    guns=g;
    size=g*25;
    initialSize=size;
    cs=isCS;
    length=size/2000.0f;
    Texture texture;
    if(cs){
        texture=CS_INF_TEXTURE_UNSEL;
    }else{
        texture=US_INF_TEXTURE_UNSEL;
    }
    button=new Button(
            x,
            y,
            0.25f,
            ICON_LENGTH,
            ICON_WIDTH,
            texture,
            (void*)changeSelected,
            (void*)&deselect,
            (void*)this
            );
    selected=false;
    hasOrder=false;
    reload=100.0/guns;
    gunDigit.x=x+ICON_LENGTH/2+NUMBER_LENGTH/2+NUMBER_OFFSET;
    gunDigit.y=y;
    gunDigit.z=0.7f;
    gunDigit.l=NUMBER_LENGTH;
    gunDigit.w=ICON_WIDTH;
    gunDigit.heapAllocated=false;
    gunDigit.visible=true;
    playerCs=pCs;
    destinationX=x;
    destinationY=y;
    updateState=(void(Unit::*)(Unit*(*)[],unsigned short))&ArtilleryBattery::artUpdateState;
    sendOrder=(void(Unit::*)(OrderType,float,float))&Regiment::regSendOrder;
    type=artillery_battery;
    allRegiments=(Unit*(*)[])regs;
    numRegiments=numRegs;
    visible=true;
    canSee=(bool(Unit::*)(Unit*))&Regiment::regCanSee;
    hills=hillArr;
    numHills=nh;
    getZ=(float(Unit::*)(float,float))&Regiment::regGetZ;
    range=2.0f;
    speed=0.000625f;
    rotationSpeed=speed/(length/2)*(180/M_PI);
    firing=false;
    limbered=true;
    limberedSprite.x=x-ICON_LENGTH/2-NUMBER_LENGTH/2-NUMBER_OFFSET;
    limberedSprite.y=y;
    limberedSprite.z=0.7f;
    limberedSprite.l=NUMBER_LENGTH;
    limberedSprite.w=ICON_WIDTH;
    limberedSprite.heapAllocated=false;
    limberedSprite.visible=true;
    limberedSprite.texture=Y;
    canFire=(bool(Regiment::*)(Unit*))&ArtilleryBattery::artCanFire;
    fireSound=createAudioSource(CANNON);
    baseReload=100.0;
}
