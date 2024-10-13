#include<iostream>
#include<string>
#include<math.h>
#include"renderer.hpp"
#define ROTATION_SPEED 1.0f
#define ACCURACY 0.055
#define NUMBER_OFFSET 0.01f
#define FOV 67.5
using namespace std;
void InfantryRegiment::infUpdateState(Unit*(*enemies)[],unsigned short enemyCount){
    if(rotation>=MAX_DEGREES)
        rotation-=MAX_DEGREES;
    if(rotation<0)
        rotation+=MAX_DEGREES;
	if(hasOrder){
        processOrder();
	}else{
		if(reload>=neededReload){
            fire(enemies,enemyCount,size*ACCURACY);
		}else{
			reload+=0.25;
			bool couldFire=false;
			for(unsigned short count=0;count<enemyCount&&!couldFire;++count){
                if((*enemies)[count]){
	    			if(regCanFire((*enemies)[count]))
    					couldFire=true;
                }
			}
			firing=couldFire;
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
			button->sprite.texture=FIRING_INF_TEXTURE_SEL;
		}else if(cs){
			button->sprite.texture=CS_INF_TEXTURE_SEL;
		}else{
			button->sprite.texture=US_INF_TEXTURE_SEL;
		}
	}else{
		if(firing){
			button->sprite.texture=FIRING_INF_TEXTURE_UNSEL;
		}else if(cs){
			button->sprite.texture=CS_INF_TEXTURE_UNSEL;
		}else{
			button->sprite.texture=US_INF_TEXTURE_UNSEL;
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
}
InfantryRegiment::InfantryRegiment(
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
    updateState=(void(Unit::*)(Unit*(*)[],unsigned short))&InfantryRegiment::infUpdateState;
    sendOrder=(void(Unit::*)(OrderType,float,float))&Regiment::regSendOrder;
    type=infantry_regiment;
    allRegiments=(Unit*(*)[])regs;
    numRegiments=numRegs;
    visible=true;
    canSee=(bool(Unit::*)(Unit*))&Regiment::regCanSee;
    hills=hillArr;
    numHills=nh;
    getZ=(float(Unit::*)(float,float))&Regiment::regGetZ;
    range=0.5f;
    speed=0.00125f;
    rotationSpeed=speed/(length/2)*(180/M_PI);
    canFire=&Regiment::regCanFire;
    fireSound=createAudioSource(MUSKET_VOLLEY);
    neededReload=getReload();
}
