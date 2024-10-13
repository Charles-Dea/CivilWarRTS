#include<iostream>
#include"renderer.hpp"
using namespace std;
Button*(*button_ptr)[]=nullptr;
unsigned short bArrLength=0;
float screenW=0;
float screenH=0;
float screenRatio=0;
Unit*(*regs)[]=nullptr;
unsigned short iArrLength=0;
Textbox*box=nullptr;
float*camX;
float*camY;
float*cameraZoom;
double previousOffset;
bool hasScrolled=false;
Scene inputScene;
//this function is called by glfw therefore the return type and parameters cannot be changed
void handleMouseInput(GLFWwindow*window,int button,int action,int mods){
	if(action==GLFW_PRESS){
		double mouseX;
		double mouseY;
		glfwGetCursorPos(window,&mouseX,&mouseY);
        float cameraX=*camX;
        float cameraY=*camY;
        double zoom=*cameraZoom;
        mouseX=(mouseX/screenW*2.0-1.0)/screenRatio/zoom+cameraX;
        mouseY=(mouseY/screenH*2.0-1.0)*-1.0/zoom+cameraY;
		if(button==GLFW_MOUSE_BUTTON_LEFT){
            if(button_ptr){
		    	bool buttonPressed=false;
	    		for(size_t count=0;count< bArrLength;++count){
                    if((*button_ptr)[count]){
                        float leftSide=(*button_ptr)[count]->x-(*button_ptr)[count]->length/2.0f;
                        float rightSide=(*button_ptr)[count]->x+(*button_ptr)[count]->length/2.0f;
                        float topSide=(*button_ptr)[count]->y+(*button_ptr)[count]->width/2.0f;
                        float bottomSide=(*button_ptr)[count]->y-(*button_ptr)[count]->width/2.0f;
        				if(
					        	mouseX>leftSide&&
				        		mouseX<rightSide&&
			        			mouseY<topSide&&
		        				mouseY>bottomSide&&
	        					!buttonPressed
        				){
					        (*button_ptr)[count]->performAction();
				        	buttonPressed=true;
			        	}else{
		        			(*button_ptr)[count]->performInvAction();
	        			}
        			}
                }
            }
		}else if(button==GLFW_MOUSE_BUTTON_RIGHT){
			for(size_t count=0;count<iArrLength;++count){
                Unit*unit=(*regs)[count];
                if(unit){
                    if(unit->selected){
                        OrderType orderType;
                        if(unit->type==infantry_regiment||unit->type==artillery_battery||unit->type==cavalry_regiment){
                            if(
                                    glfwGetKey(window,GLFW_KEY_LEFT_SHIFT)==GLFW_PRESS||
                                    glfwGetKey(window,GLFW_KEY_RIGHT_SHIFT)==GLFW_PRESS
                              ){
                                orderType=rotate;
                            }else{
                                orderType=march;
                            }
                        }else if(unit->type==brigade){
                            if(
                                    glfwGetKey(window,GLFW_KEY_LEFT_CONTROL)==GLFW_PRESS||
                                    glfwGetKey(window,GLFW_KEY_RIGHT_CONTROL)==GLFW_PRESS
                              ){
                                orderType=commanderMarch;
                            }else{
                                orderType=march;
                            }
                        }
                        (*unit.*unit->sendOrder)(orderType,mouseX,mouseY);
                        float fCount=count;
                        float fOrderType=orderType;
                        float destinationX=mouseX;
                        float destinationY=mouseY;
                        float data[]={fCount,destinationX,destinationY,fOrderType};
                        sendOrder(&data);
                    }
    			}
			}
		}
	}
}
//this function is called by glfw therefore the return type and parameters cannot be changed
void handleKeyInput(GLFWwindow*window,int key,int scancode,int action,int mods){
    if(action==GLFW_PRESS){
	    if(key==GLFW_KEY_ESCAPE){
            loadMainMenu();
        }else if(key=='M'){
            for(unsigned short count=0;count<iArrLength;++count){
                if((*regs)[count]&&(*regs)[count]->selected){
                    if((*regs)[count]->type==artillery_battery){
                        ArtilleryBattery*battery=(ArtilleryBattery*)(*regs)[count];
                        battery->limbered=!battery->limbered;
                        SetLimbered data={.i=count,.limbered=battery->limbered};
                        setLimbered(&data);
                    }else if((*regs)[count]->type==cavalry_regiment){
                        CavalryRegiment*reg=(CavalryRegiment*)(*regs)[count];
                        reg->mounted=!reg->mounted;
                        SetLimbered data={.i=count,.limbered=reg->mounted};
                        setLimbered(&data);
                    }else if((*regs)[count]->type==brigade){
                        Brigade*brigade=(Brigade*)(*regs)[count];
                        unsigned short limbered=0,unlimbered=0;
                        unsigned short numRegs=brigade->regs;
                        Regiment*(*batteries)[]=brigade->regiments;
                        for(unsigned short i=0;i<numRegs;++i){
                            if((*batteries)[i]){
                                if((*batteries)[i]->type==artillery_battery){
                                    ArtilleryBattery*battery=(ArtilleryBattery*)(*batteries)[i];
                                    if(battery->limbered){
                                        ++limbered;
                                    }else{
                                        ++unlimbered;
                                    }
                                }else if((*batteries)[i]->type==cavalry_regiment){
                                    CavalryRegiment*reg=(CavalryRegiment*)(*batteries)[i];
                                    if(reg->mounted){
                                        ++limbered;
                                    }else{
                                        ++unlimbered;
                                    }
                                }
                            }
                        }
                        bool majority_unlimbered=unlimbered>limbered;
                        for(unsigned short i=0;i<numRegs;++i){
                            if((*batteries)[i]){
                                if((*batteries)[i]->type==artillery_battery){
                                    ArtilleryBattery*battery=(ArtilleryBattery*)(*batteries)[i];
                                    battery->limbered=majority_unlimbered;
                                    unsigned short battery_index;
                                    for(unsigned short idx=0;idx<iArrLength;++idx){
                                        if((*regs)[idx]==battery){
                                            battery_index=idx;
                                            break;
                                        }
                                    }
                                    SetLimbered data={.i=battery_index,.limbered=majority_unlimbered};
                                    setLimbered(&data);
                                }else if((*batteries)[i]->type==cavalry_regiment){
                                    CavalryRegiment*reg=(CavalryRegiment*)(*batteries)[i];
                                    reg->mounted=majority_unlimbered;
                                    unsigned short battery_index;
                                    for(unsigned short idx=0;idx<iArrLength;++idx){
                                        if((*regs)[idx]==reg){
                                            battery_index=idx;
                                            break;
                                        }
                                    }
                                    SetLimbered data={.i=battery_index,.limbered=majority_unlimbered};
                                    setLimbered(&data);
                                }
                            }
                        }
                    }
                }
            }
        }else{
            if(box){
                if(key==GLFW_KEY_ENTER&&box->setChars!=0){
                    box->func((char*)box->text);
                }else if(key==GLFW_KEY_BACKSPACE&&box->setChars!=0){
                    --box->setChars;
                    (*box->text)[box->setChars]=NULL;
                    box->displayString();
                }else if(((key>='0'&&key<='9')||key=='.')&&box->setChars<box->cells){
                    (*box->text)[box->setChars]=key;
                    ++box->setChars;
                    box->displayString();
                }
            }
            if(key==GLFW_KEY_SPACE&&regs){
                for(unsigned short count=0;count<iArrLength;++count){
                    if((*regs)[count]){
                        if((*regs)[count]->selected){
                            Unit*unit=(*regs)[count];
                            (*unit.*unit->sendOrder)(halt,NULL,NULL);
                            orderHalt(&count);
                        }
                    }
                }
            }
        }
    }
}
void handleScrollInput(GLFWwindow*window,double xOffset,double yOffset){
    if(inputScene==battlefield)
        *cameraZoom+=yOffset*0.1;
}
void initInput(GLFWwindow*window,float*cameraX,float*cameraY,float*zoom){
	glfwSetMouseButtonCallback(window,handleMouseInput);
	glfwSetKeyCallback(window,handleKeyInput);
    glfwSetScrollCallback(window,&handleScrollInput);
	const GLFWvidmode*vidMode=glfwGetVideoMode(glfwGetPrimaryMonitor());
	screenW=vidMode->width;
	screenH=vidMode->height;
	screenRatio=screenH/screenW;
    camX=cameraX;
    camY=cameraY;
    cameraZoom=zoom;
}
void updateForScene(
        Unit*(*r)[],
        Button*(*b)[],
        const unsigned short bl,
        const unsigned short il,
        Textbox*textbox,
        Scene scene
        ){
	regs=r;
	button_ptr=b;
	bArrLength=bl;
	iArrLength=il;
    box=textbox;
    inputScene=scene;
}
