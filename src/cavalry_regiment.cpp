#include<iostream>
#include<string>
#include<math.h>
#include"renderer.hpp"
#define ROTATION_SPEED 1.0f
#define ACCURACY 0.0275
#define NUMBER_OFFSET 0.01f
#define FOV 67.5
using namespace std;
bool CavalryRegiment::cavCanFire(Unit*target){
    return mounted?false:regCanFire(target);
}
void CavalryRegiment::cavUpdateState(Unit*(*enemies)[],unsigned short enemyCount){
    if(rotation>=MAX_DEGREES)
        rotation-=MAX_DEGREES;
    if(rotation<0)
        rotation+=MAX_DEGREES;
	if(hasOrder){
        processOrder();
	}else{
        firing=false;
        for(unsigned short count=0;count<enemyCount&&!firing;++count){
            if((*enemies)[count])
                firing=regCanFire((*enemies)[count]);
        }
        if(firing)
            fire(enemies,enemyCount,size*ACCURACY/200.0);
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
			button->sprite.texture=FIRING_CAV_TEXTURE_SEL;
		}else if(cs){
			button->sprite.texture=CS_CAV_TEXTURE_SEL;
		}else{
			button->sprite.texture=US_CAV_TEXTURE_SEL;
		}
	}else{
		if(firing){
			button->sprite.texture=FIRING_CAV_TEXTURE_UNSEL;
		}else if(cs){
			button->sprite.texture=CS_CAV_TEXTURE_UNSEL;
		}else{
			button->sprite.texture=US_CAV_TEXTURE_UNSEL;
		}
	}
	hunDigit.x=xPosition+ICON_LENGTH/2+NUMBER_LENGTH/2+NUMBER_OFFSET;
	tenDigit.x=hunDigit.x+NUMBER_LENGTH+NUMBER_OFFSET;
	oneDigit.x=tenDigit.x+NUMBER_LENGTH+NUMBER_OFFSET;
	hunDigit.y=yPosition;
	tenDigit.y=yPosition;
	oneDigit.y=yPosition;
	unsigned short hunDigitNum=size/100;
	unsigned short tenDigitNum=(size-hunDigitNum*100)/10;
	unsigned short oneDigitNum=size-hunDigitNum*100-tenDigitNum*10;
	hunDigit.texture=getNumSprite(hunDigitNum);
	tenDigit.texture=getNumSprite(tenDigitNum);
	oneDigit.texture=getNumSprite(oneDigitNum);
    mountedSprite.x=xPosition-ICON_LENGTH/2-NUMBER_LENGTH/2-NUMBER_OFFSET;
    mountedSprite.y=yPosition;
    mountedSprite.texture=mounted?Y:N;
    speed=mounted?0.0025f:0.00125f;
}
CavalryRegiment::CavalryRegiment(
        float x,
        float y,
        float r,
        float s,
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
    size=s;
    initialSize=size;
    cs=isCS;
    length=size/2000.0f;
    Texture texture;
    if(cs){
        texture=CS_CAV_TEXTURE_UNSEL;
    }else{
        texture=US_CAV_TEXTURE_UNSEL;
    }
    button=new Button(
        x,
        y,
		0.25f,
        ICON_LENGTH,
        ICON_WIDTH,
        texture,
        (void*)&changeSelected,
        (void*)&deselect,
        (void*)this
    );
    selected=false;
	hasOrder=false;
	firing=false;
	reload=100;
	hunDigit.x=xPosition+ICON_LENGTH/2+NUMBER_LENGTH/2+NUMBER_OFFSET;
	tenDigit.x=hunDigit.x+NUMBER_LENGTH+NUMBER_OFFSET;
	oneDigit.x=tenDigit.x+NUMBER_LENGTH+NUMBER_OFFSET;
	hunDigit.y=yPosition;
	tenDigit.y=yPosition;
	oneDigit.y=yPosition;
	hunDigit.z=0.7;
	tenDigit.z=0.7;
	oneDigit.z=0.7;
	hunDigit.l=NUMBER_LENGTH;
	tenDigit.l=NUMBER_LENGTH;
	oneDigit.l=NUMBER_LENGTH;
	hunDigit.w=ICON_WIDTH;
	tenDigit.w=ICON_WIDTH;
	oneDigit.w=ICON_WIDTH;
	hunDigit.heapAllocated=false;
	tenDigit.heapAllocated=false;
	oneDigit.heapAllocated=false;
	hunDigit.visible=true;
	tenDigit.visible=true;
	oneDigit.visible=true;
    playerCs=pCs;
    destinationX=xPosition;
    destinationY=yPosition;
    updateState=(void(Unit::*)(Unit*(*)[],unsigned short))&CavalryRegiment::cavUpdateState;
    sendOrder=(void(Unit::*)(OrderType,float,float))&Regiment::regSendOrder;
    type=cavalry_regiment;
    allRegiments=(Unit*(*)[])regs;
    numRegiments=numRegs;
    visible=true;
    canSee=(bool(Unit::*)(Unit*))&Regiment::regCanSee;
    hills=hillArr;
    numHills=nh;
    getZ=(float(Unit::*)(float,float))&Regiment::regGetZ;
    range=0.25f;
    speed=0.0025f;
    rotationSpeed=speed/(length/2)*(180/M_PI);
    canFire=(bool(Regiment::*)(Unit*))&CavalryRegiment::cavCanFire;
    mounted=true;
    mountedSprite.x=x-ICON_LENGTH/2-NUMBER_LENGTH/2-NUMBER_OFFSET;
    mountedSprite.y=y;
    mountedSprite.z=0.7f;
    mountedSprite.l=NUMBER_LENGTH;
    mountedSprite.w=ICON_WIDTH;
    mountedSprite.heapAllocated=false;
    mountedSprite.visible=true;
    fireSound=createAudioSource(MUSKET_VOLLEY);
    neededReload=getReload();
}
