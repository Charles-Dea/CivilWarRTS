#include<iostream>
#include<malloc.h>
#include"renderer.hpp"
using namespace std;
void Textbox::displayString(){
    for(unsigned short count=0;count<cells;++count){
        char character=(*text)[count];
        if(character>='0'&&character<='9'){
            (*chars)[count].texture=ZERO+character-'0';
        }else if(character=='.'){
            (*chars)[count].texture=DOT;
        }else{
            (*chars)[count].texture=NULL_TEXTURE;
        }
        /*if(character=='0'){
            (*chars)[count].texture=getTexture(zero);
        }else if(character=='1'){
            (*chars)[count].texture=getTexture(one);
        }else if(character=='2'){
            (*chars)[count].texture=getTexture(two);
        }else if(character=='3'){
            (*chars)[count].texture=getTexture(three);
        }else if(character=='4'){
            (*chars)[count].texture=getTexture(four);
        }else if(character=='5'){
            (*chars)[count].texture=getTexture(five);
        }else if(character=='6'){
            (*chars)[count].texture=getTexture(six);
        }else if(character=='7'){
            (*chars)[count].texture=getTexture(seven);
        }else if(character=='8'){
            (*chars)[count].texture=getTexture(eight);
        }else if(character=='9'){
            (*chars)[count].texture=getTexture(nine);
        }else if(character=='.'){
            (*chars)[count].texture=getTexture(dot);
        }else{
            (*chars)[count].texture=NULL;
        }*/
    }
}
Textbox::Textbox(float xPos,float yPos,short c,void(*f)(char*)){
    unsigned short textLength=c+1;
    text=(char(*)[])malloc(sizeof(char[textLength]));
    for(unsigned short count=0;count<textLength;++count){
        (*text)[count]=NULL;
    }
    x=xPos;
    y=yPos;
    cells=c;
    length=0.1f*cells;
    height=0.2f;
    box.x=x;
    box.y=y;
    box.z=1.0f;
    box.l=length;
    box.w=height;
    box.texture=BOX_TEXTURE;
    box.heapAllocated=false;
    box.visible=true;
    chars=(Sprite(*)[])malloc(sizeof(Sprite[cells]));//I am fully aware malloc and c-style casting are unsafe I just don't care
    for(short count;count<cells;++count){
        (*chars)[count].x=xPos+(0.0f-cells/2+count)*0.1f;
        (*chars)[count].y=yPos;
        (*chars)[count].z=0.0f;
        (*chars)[count].l=0.09f;
        (*chars)[count].w=0.2f;
        (*chars)[count].texture=NULL;
        (*chars)[count].heapAllocated=false;
        (*chars)[count].visible=true;
    }
    func=f;
    setChars=0;
}
