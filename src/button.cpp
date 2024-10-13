#include<iostream>
#include"unit.hpp"
void Button::performAction(){
	//check if button is icon for regiment
	if(parent!=nullptr){
		void(*temp)(Unit*)=(void(*)(Unit*))func;
		temp((Unit*)parent);
	}else{
		void(*temp)()=(void(*)())func;
		temp();
	}
}
//this function only exists so regiments are deselected when left click occurs outside of button
void Button::performInvAction(){
	if(invFunc!=nullptr){
		if(parent!=nullptr){
			void(*temp)(Unit*)=(void(*)(Unit*))invFunc;
			temp((Unit*)parent);
		}else{
			void(*temp)()=(void(*)())invFunc;
			temp();
		}
	}
}
Button::Button(float xPos,float yPos,float zPos,float l,float w,unsigned int t,void*f,void*iF,void*p){
    x=xPos;
    y=yPos;
    length=l;
    width=w;
	sprite.x=xPos;
	sprite.y=yPos;
	sprite.z=zPos;
	sprite.l=l;
	sprite.w=w;
    sprite.texture=t;
    sprite.heapAllocated=false;
    sprite.visible=true;
    func=f;
	invFunc=iF;
    parent=p;
}
