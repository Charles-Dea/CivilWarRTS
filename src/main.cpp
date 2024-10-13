#include<AL/al.h>
#include<enet/enet.h>
#include<iostream>
#include<chrono>
#include<thread>
#include<malloc.h>
#include"renderer.hpp"
#define MAIN_MENU 0
#define BATTLEFIELD 1
using namespace std;
using namespace chrono;
Scene scene=mainMenu;
Unit*(*units)[]=nullptr;
Regiment*(*regiments)[]=nullptr;
Unit*(*usUnits)[]=nullptr;
Unit*(*csUnits)[]=nullptr;
Regiment*(*usRegiments)[]=nullptr;
Regiment*(*csRegiments)[]=nullptr;
Button*(*buttons)[]=nullptr;
Sprite*(*sprites)[]=nullptr;
Hill(*hills)[]=nullptr;
Textbox*textbox=nullptr;
unsigned short unitArrLength=0;
unsigned short infArrLength=0;
unsigned short usUnitsLength=0;
unsigned short csUnitsLength=0;
unsigned short usRegs=0;
unsigned short csRegs=0;
unsigned short spriteArrLength=0;
unsigned short buttonArrLength=0;
unsigned short loadedUnits=0;
unsigned short loadedUsUnits=0;
unsigned short loadedCsUnits=0;
unsigned short loadedUsRegs=0;
unsigned short loadedCsRegs=0;
unsigned short loadedButtons=0;
unsigned short loadedSprites=0;
unsigned short numHills=0;
GLFWwindow*mainWindow;
bool sceneLoaded=false;
bool exited=false;
bool playerCs=false;
bool playerCsSet=false;
NetworkStatus networkStatus=disconnected;
bool hasSetupHost=false;
bool loadingScene=false;
float sr;
void unloadScene(){
    pauseRendering();
	if(sprites){
        for(unsigned short count=0;count<spriteArrLength;++count){
            if((*sprites)[count]){
                if((*sprites)[count]->heapAllocated)
                    delete (*sprites)[count];
            }
        }
		free(sprites);
		sprites=nullptr;
        spriteArrLength=0;
        loadedSprites=0;
	}
	if(units){
		for(unsigned short count=0;count<unitArrLength;++count){
            if((*units)[count]){
                if((*units)[count]->type==brigade){
                    Brigade*brigade=(Brigade*)(*units)[count];
                    free(brigade->regiments);
                }else{
                    Regiment*regiment=(Regiment*)(*units)[count];
                    alDeleteSources(1,&regiment->fireSound);
                }
	    		delete (*units)[count];
    		}
		}
		free(units);
		units=nullptr;
        unitArrLength=0;
        loadedUnits=0;
	}
    if(regiments){
        free(regiments);
        regiments=nullptr;
        infArrLength=0;
    }
	if(buttons){
		for(unsigned short count=0;count<buttonArrLength;++count){
			delete (*buttons)[count];
		}
		free(buttons);
		buttons=nullptr;
        buttonArrLength=0;
        loadedButtons=0;
	}
	if(usUnits){
		free(usUnits);
		usUnits=nullptr;
	    usUnitsLength=0;
        loadedUsUnits=0;
	}
    if(csUnits){
		free(csUnits);
		csUnits=nullptr;
	    csUnitsLength=0;
        loadedCsUnits=0;
    }
	if(usRegiments){
		free(usRegiments);
		usRegiments=nullptr;
	    usRegs=0;
        loadedUsRegs=0;
	}
    if(csRegiments){
		free(csRegiments);
		csRegiments=nullptr;
	    csRegs=0;
        loadedCsRegs=0;
    }
    if(hills){
        free(hills);
        hills=nullptr;
        numHills=0;
    }
    if(textbox){
        delete textbox;
        textbox=nullptr;
    }
}
void loadVictoryScreen(Texture texture){
    loadingScene=true;
    unloadScene();
    setBackground(0.0f,0.0f,0.0f);
    infArrLength=0;
    spriteArrLength=1;
    sprites=(Sprite*(*)[])malloc(sizeof(Sprite*[spriteArrLength]));
    (*sprites)[0]=new Sprite;
    (*sprites)[0]->x=0.0f;
    (*sprites)[0]->y=0.0f;
    (*sprites)[0]->z=0.0f;
    (*sprites)[0]->l=2.0f;
    (*sprites)[0]->w=1.0f;
    (*sprites)[0]->texture=texture;
    (*sprites)[0]->heapAllocated=true;
    (*sprites)[0]->visible=true;
    scene=victoryScreen;
	updateForScene(units,buttons,buttonArrLength,unitArrLength,textbox,scene);
    updateRenderer(usRegiments,usRegs,csRegiments,csRegs,sprites,spriteArrLength,scene,nullptr,0);
    setRegs(units);
    loadingScene=false;
}
void loadRegiment(Regiment*regiment){
    (*units)[loadedUnits]=regiment;
    ++loadedUnits;
    if(regiment->cs){
        (*csUnits)[loadedCsUnits]=regiment;
        ++loadedCsUnits;
        (*csRegiments)[loadedCsRegs]=regiment;
        ++loadedCsRegs;
    }else{
        (*usUnits)[loadedUsUnits]=regiment;
        ++loadedUsUnits;
        (*usRegiments)[loadedUsRegs]=regiment;
        ++loadedUsRegs;
    }
    (*buttons)[loadedButtons]=regiment->button;
    (*sprites)[loadedSprites]=&(*buttons)[loadedButtons]->sprite;
    ++loadedButtons;
    ++loadedSprites;
}
InfantryRegiment*loadInfRegiment(float x,float y,float rotation,unsigned short size,bool cs){
    InfantryRegiment*regiment=new InfantryRegiment(
            x,
            y,
            rotation,
            size,
            cs,
            &playerCs,
            regiments,
            infArrLength,
            hills,
            numHills
            );
    loadRegiment(regiment);
    (*sprites)[loadedSprites]=&regiment->hunDigit;
    ++loadedSprites;
    (*sprites)[loadedSprites]=&regiment->tenDigit;
    ++loadedSprites;
    (*sprites)[loadedSprites]=&regiment->oneDigit;
    ++loadedSprites;
    return regiment;
}
CavalryRegiment*loadCavRegiment(float x,float y,float rotation,unsigned short size,bool cs){
    CavalryRegiment*regiment=new CavalryRegiment(
            x,
            y,
            rotation,
            size,
            cs,
            &playerCs,
            regiments,
            infArrLength,
            hills,
            numHills
            );
    loadRegiment(regiment);
    (*sprites)[loadedSprites]=&regiment->hunDigit;
    ++loadedSprites;
    (*sprites)[loadedSprites]=&regiment->tenDigit;
    ++loadedSprites;
    (*sprites)[loadedSprites]=&regiment->oneDigit;
    ++loadedSprites;
    (*sprites)[loadedSprites]=&regiment->mountedSprite;
    ++loadedSprites;
    return regiment;
}
ArtilleryBattery*loadBattery(float x,float y,float rotation,unsigned short guns,bool cs){
    ArtilleryBattery*battery=new ArtilleryBattery(x,y,rotation,guns,cs,&playerCs,regiments,infArrLength,hills,numHills);
    loadRegiment(battery);
    (*sprites)[loadedSprites]=&battery->gunDigit;
    ++loadedSprites;
    (*sprites)[loadedSprites]=&battery->limberedSprite;
    ++loadedSprites;
    return battery;
}
Brigade*loadBrigade(float x,float y,float rotation,float size,bool cs,unsigned short index,unsigned short numRegs){
    Regiment*(*regs)[]=(Regiment*(*)[])malloc(sizeof(Regiment*[numRegs]));
    for(unsigned short count=0;count<numRegs;++count){
        (*regs)[count]=(*regiments)[index+count];
    }
    Brigade*brigade=new Brigade(x,y,rotation,size,cs,regs,numRegs,&playerCs,regiments,infArrLength,hills,numHills);
    if(cs){
        (*csUnits)[loadedCsUnits]=brigade;
        ++loadedCsUnits;
    }else{
        (*usUnits)[loadedUsUnits]=brigade;
        ++loadedUsUnits;
    }
    (*buttons)[loadedButtons]=brigade->button;
    (*sprites)[loadedSprites]=&(*buttons)[loadedButtons]->sprite;
    ++loadedButtons;
    ++loadedSprites;
    (*sprites)[loadedSprites]=&brigade->thouDigit;
    ++loadedSprites;
    (*sprites)[loadedSprites]=&brigade->hunDigit;
    ++loadedSprites;
    (*sprites)[loadedSprites]=&brigade->tenDigit;
    ++loadedSprites;
    (*sprites)[loadedSprites]=&brigade->oneDigit;
    ++loadedSprites;
    return brigade;
}
void loadBattlefield(){
    loadingScene=true;
	unloadScene();
    setBackground(0.0f,0.5f,0.0f);
    networkStatus=connected;
    unitArrLength=72;
    units=(Unit*(*)[])malloc(sizeof(Unit*[unitArrLength]));
    infArrLength=60;
	regiments=(Regiment*(*)[])malloc(sizeof(Regiment*[infArrLength]));
    usUnitsLength=36;
    usUnits=(Unit*(*)[])malloc(sizeof(Unit*[usUnitsLength]));
    csUnitsLength=36;
    csUnits=(Unit*(*)[])malloc(sizeof(Unit*[csUnitsLength]));
    usRegs=30;
    usRegiments=(Regiment*(*)[])malloc(sizeof(Regiment*[usRegs]));
    csRegs=30;
    csRegiments=(Regiment*(*)[])malloc(sizeof(Regiment*[csRegs]));
    buttonArrLength=unitArrLength;
    buttons=(Button*(*)[])malloc(sizeof(Button*[buttonArrLength]));
    spriteArrLength=300;
    sprites=(Sprite*(*)[])malloc(sizeof(Sprite*[spriteArrLength]));
    numHills=5;
    hills=(Hill(*)[])malloc(sizeof(Hill[numHills]));
    (*hills)[0]=Hill(-2.0f,2.0f,1.0f,0.5f,2.0f,0.0f);
    (*hills)[1]=Hill(0.0f,0.0f,2.0f,0.5f,1.0f,0.0f);
    (*hills)[2]=Hill(2.0f,2.0f,1.0f,0.5f,1.0f,12.5f);
    (*hills)[3]=Hill(-2.0f,-2.0f,0.25f,0.5f,0.5f,0.0f);
    (*hills)[4]=Hill(2.0f,-2.0f,1.0f,0.5f,1.0f,45.0f);
    (*regiments)[0]=loadInfRegiment(-0.9f,0.9f,0.0f,500,false);
    (*regiments)[1]=loadInfRegiment(-0.7f,0.9f,0.0f,500,false);
    (*regiments)[2]=loadInfRegiment(-0.5f,0.9f,0.0f,500,false);
    (*regiments)[3]=loadInfRegiment(-0.3f,0.9f,0.0f,500,false);
    (*regiments)[4]=loadInfRegiment(-0.1f,0.9f,0.0f,500,false);
    (*regiments)[5]=loadInfRegiment(0.1f,0.9f,0.0f,500,false);
    (*regiments)[6]=loadInfRegiment(0.3f,0.9f,0.0f,500,false);
    (*regiments)[7]=loadInfRegiment(0.5f,0.9f,0.0f,500,false);
    (*regiments)[8]=loadInfRegiment(0.7f,0.9f,0.0f,500,false);
    (*regiments)[9]=loadInfRegiment(0.9F,0.9f,0.0f,500,false);
    (*regiments)[10]=loadInfRegiment(-0.9f,0.8f,0.0f,500,false);
    (*regiments)[11]=loadInfRegiment(-0.7f,0.8f,0.0f,500,false);
    (*regiments)[12]=loadInfRegiment(-0.5f,0.8f,0.0f,500,false);
    (*regiments)[13]=loadInfRegiment(-0.3f,0.8f,0.0f,500,false);
    (*regiments)[14]=loadInfRegiment(-0.1f,0.8f,0.0f,500,false);
    (*regiments)[15]=loadInfRegiment(0.1f,0.8f,0.0f,500,false);
    (*regiments)[16]=loadInfRegiment(0.3f,0.8f,0.0f,500,false);
    (*regiments)[17]=loadInfRegiment(0.5f,0.8f,0.0f,500,false);
    (*regiments)[18]=loadInfRegiment(0.7f,0.8f,0.0f,500,false);
    (*regiments)[19]=loadInfRegiment(0.9F,0.8f,0.0f,500,false);
    (*regiments)[20]=loadInfRegiment(-0.9f,-0.9f,180.0f,500,true);
    (*regiments)[21]=loadInfRegiment(-0.7f,-0.9f,180.0f,500,true);
    (*regiments)[22]=loadInfRegiment(-0.5f,-0.9f,180.0f,500,true);
    (*regiments)[23]=loadInfRegiment(-0.3f,-0.9f,180.0f,500,true);
    (*regiments)[24]=loadInfRegiment(-0.1f,-0.9f,180.0f,500,true);
    (*regiments)[25]=loadInfRegiment(0.1f,-0.9f,180.0f,500,true);
    (*regiments)[26]=loadInfRegiment(0.3f,-0.9f,180.0f,500,true);
    (*regiments)[27]=loadInfRegiment(0.5f,-0.9f,180.0f,500,true);
    (*regiments)[28]=loadInfRegiment(0.7f,-0.9f,180.0f,500,true);
    (*regiments)[29]=loadInfRegiment(0.9f,-0.9f,180.0f,500,true);
    (*regiments)[30]=loadInfRegiment(-0.9f,-0.8f,180.0f,500,true);
    (*regiments)[31]=loadInfRegiment(-0.7f,-0.8f,180.0f,500,true);
    (*regiments)[32]=loadInfRegiment(-0.5f,-0.8f,180.0f,500,true);
    (*regiments)[33]=loadInfRegiment(-0.3f,-0.8f,180.0f,500,true);
    (*regiments)[34]=loadInfRegiment(-0.1f,-0.8f,180.0f,500,true);
    (*regiments)[35]=loadInfRegiment(0.1f,-0.8f,180.0f,500,true);
    (*regiments)[36]=loadInfRegiment(0.3f,-0.8f,180.0f,500,true);
    (*regiments)[37]=loadInfRegiment(0.5f,-0.8f,180.0f,500,true);
    (*regiments)[38]=loadInfRegiment(0.7f,-0.8f,180.0f,500,true);
    (*regiments)[39]=loadInfRegiment(0.9f,-0.8f,180.0f,500,true);
    (*regiments)[40]=loadBattery(0.9f,1.0f,0.0f,6,false);
    (*regiments)[41]=loadBattery(0.45f,1.0f,0.0f,6,false);
    (*regiments)[42]=loadBattery(0.0f,1.0f,0.0f,6,false);
    (*regiments)[43]=loadBattery(-0.45f,1.0f,0.0f,6,false);
    (*regiments)[44]=loadBattery(-0.9f,1.0f,0.0f,6,false);
    (*regiments)[45]=loadBattery(0.9f,-1.0f,180.0f,6,true);
    (*regiments)[46]=loadBattery(0.45f,-1.0f,180.0f,6,true);
    (*regiments)[47]=loadBattery(0.0f,-1.0f,180.0f,6,true);
    (*regiments)[48]=loadBattery(-0.45f,-1.0f,180.0f,6,true);
    (*regiments)[49]=loadBattery(-0.9f,-1.0f,180.0f,6,true);
    (*regiments)[50]=loadCavRegiment(0.9f,0.7f,0.0f,250,false);
    (*regiments)[51]=loadCavRegiment(0.45f,0.7f,0.0f,250,false);
    (*regiments)[52]=loadCavRegiment(0.0f,0.7f,0.0f,250,false);
    (*regiments)[53]=loadCavRegiment(-0.45f,0.7f,0.0f,250,false);
    (*regiments)[54]=loadCavRegiment(-0.9f,0.7f,0.0f,250,false);
    (*regiments)[55]=loadCavRegiment(0.9f,-0.7f,180.0f,250,true);
    (*regiments)[56]=loadCavRegiment(0.45,-0.7f,180.0f,250,true);
    (*regiments)[57]=loadCavRegiment(0.0f,-0.7f,180.0f,250,true);
    (*regiments)[58]=loadCavRegiment(-0.45,-0.7f,180.0f,250,true);
    (*regiments)[59]=loadCavRegiment(-0.9f,-0.7f,180.0f,250,true);
    (*units)[loadedUnits]=loadBrigade(-0.5f,0.95f,0.0f,50,false,0,5);
    ++loadedUnits;
    (*units)[loadedUnits]=loadBrigade(0.5f,0.95f,0.0f,50,false,5,5);
    ++loadedUnits;
    (*units)[loadedUnits]=loadBrigade(-0.5f,0.85f,0.0f,50,false,10,5);
    ++loadedUnits;
    (*units)[loadedUnits]=loadBrigade(0.5f,0.85f,0.0f,50,false,15,5);
    ++loadedUnits;
    (*units)[loadedUnits]=loadBrigade(-0.5f,-0.95f,180.0f,50,true,20,5);
    ++loadedUnits;
    (*units)[loadedUnits]=loadBrigade(0.5f,-0.95f,180.0f,50,true,25,5);
    ++loadedUnits;
    (*units)[loadedUnits]=loadBrigade(-0.5f,-0.85f,180.0f,50,true,30,5);
    ++loadedUnits;
    (*units)[loadedUnits]=loadBrigade(0.5f,-0.85f,180.0f,50,true,35,5);
    ++loadedUnits;
    (*units)[loadedUnits]=loadBrigade(0.0f,1.1f,0.0f,50,false,40,5);
    ++loadedUnits;
    (*units)[loadedUnits]=loadBrigade(0.0f,-1.1f,180.0f,50,true,45,5);
    ++loadedUnits;
    (*units)[loadedUnits]=loadBrigade(0.0f,0.75f,0.0f,50,false,50,5);
    ++loadedUnits;
    (*units)[loadedUnits]=loadBrigade(0.0f,-0.75f,180.0f,50,true,55,5);
    ++loadedUnits;
    if(loadedUnits<unitArrLength)
        cout<<unitArrLength-loadedUnits<<" unloaded units\n";
    if(loadedUsUnits<usUnitsLength)
        printf("ERROR: not all us units loaded\n");
    if(loadedCsUnits<usUnitsLength)
        printf("ERROR: not all cs units loaded\n");
    if(loadedUsRegs<usRegs)
        printf("ERROR: not all us regiments loaded\n");
    if(loadedCsRegs<csRegs)
        printf("ERROR: not all cs regiments loaded\n");
    if(loadedButtons<buttonArrLength)
        printf("ERROR: not all buttons loaded\n");
    if(loadedSprites<spriteArrLength)
        printf("ERROR: not all sprites loaded\n");
    scene=battlefield;
	updateForScene(units,buttons,buttonArrLength,unitArrLength,textbox,scene);
    updateRenderer(usRegiments,usRegs,csRegiments,csRegs,sprites,spriteArrLength,scene,hills,numHills);
    setRegs(units);
    loadingScene=false;
}
void waitingScreen(){
    unloadScene();
    networkStatus=waiting;
    spriteArrLength=1;
    sprites=reinterpret_cast<Sprite*(*)[]>(malloc(sizeof(Sprite*[spriteArrLength])));
    (*sprites)[0]=new Sprite;
    (*sprites)[0]->x=0.0f;
    (*sprites)[0]->y=0.0f;
    (*sprites)[0]->l=2.0f;
    (*sprites)[0]->w=0.5f;
    (*sprites)[0]->texture=WAITING_FOR_PLAYER;
    (*sprites)[0]->heapAllocated=true;
    (*sprites)[0]->visible=true;
    hasSetupHost=false;
    scene=waitingForPlayerScreen;
    updateForScene(units,buttons,buttonArrLength,unitArrLength,textbox,scene);
    updateRenderer(usRegiments,usRegs,csRegiments,csRegs,sprites,spriteArrLength,scene,nullptr,0);
}
void hostUsGame(){
    loadingScene=true;
    playerCsSet=true;
    playerCs=false;
    waitingScreen();
    loadingScene=false;
}
void hostCsGame(){
    loadingScene=true;
    playerCsSet=true;
    playerCs=true;
    waitingScreen();
    loadingScene=false;
}
void hostGame(){
    loadingScene=true;
    unloadScene();
    buttons=reinterpret_cast<Button*(*)[]>(malloc(sizeof(Button*[2])));
    (*buttons)[0]=new Button(
            -0.5f,
            0.0f,
            1.0f,
            0.5f,
            0.25f,
            US_FLAG,
            reinterpret_cast<void*>(&hostUsGame),
            nullptr,
            nullptr
            );
    (*buttons)[1]=new Button(
            0.5f,
            0.0f,
            1.0f,
            0.5f,
            0.25f,
            CS_FLAG,
            reinterpret_cast<void*>(&hostCsGame),
            nullptr,
            nullptr
            );
    buttonArrLength=2;
    sprites=reinterpret_cast<Sprite*(*)[]>(malloc(sizeof(Sprite*[3])));
    (*sprites)[0]=&(*buttons)[0]->sprite;
    (*sprites)[1]=&(*buttons)[1]->sprite;
    (*sprites)[2]=new Sprite;
    (*sprites)[2]->x=0.0f;
    (*sprites)[2]->y=0.5f;
    (*sprites)[2]->l=1.5f;
    (*sprites)[2]->w=0.75f;
    (*sprites)[2]->texture=SELECT_SIDE;
    (*sprites)[2]->heapAllocated=true;
    (*sprites)[2]->visible=true;
    spriteArrLength=3;
    scene=sideSelection;
    updateForScene(units,buttons,buttonArrLength,unitArrLength,textbox,scene);
    updateRenderer(usRegiments,usRegs,csRegiments,csRegs,sprites,spriteArrLength,scene,nullptr,0);
    loadingScene=false;
}
void couldNotConnectScreen(){
    loadingScene=true;
    unloadScene();
    setBackground(0.0f,0.0f,0.0f);
    spriteArrLength=1;
    sprites=(Sprite*(*)[])malloc(sizeof(Sprite*[spriteArrLength]));
    Sprite*sprite=new Sprite;
    sprite->x=0.0f;
    sprite->y=0.0f;
    sprite->z=0.0f;
    sprite->l=2.0f;
    sprite->w=1.0f;
    sprite->texture=COULD_NOT_CONNECT;
    sprite->heapAllocated=true;
    sprite->visible=true;
    (*sprites)[0]=sprite;
    scene=couldNotConnectScene;
    updateForScene(nullptr,nullptr,0,0,nullptr,scene);
    updateRenderer(nullptr,0,nullptr,0,sprites,spriteArrLength,scene,nullptr,0);
    loadingScene=false;
}
void joinGame(char*text){
    loadBattlefield();
    loadingScene=true;//loadbattlefield sets loadingScene to false when it ends
    if(!setupGuest(units,text))
        couldNotConnectScreen();
    loadingScene=false;
}
void inputIp(){
    loadingScene=true;
    unloadScene();
    const unsigned short cells=15;
    textbox=new Textbox(0.0f,0.0f,cells,&joinGame);
    spriteArrLength=cells+2;
    sprites=(Sprite*(*)[])malloc(sizeof(Sprite*[spriteArrLength]));
    (*sprites)[0]=&textbox->box;
    (*sprites)[1]=&(*textbox->chars)[0];
    (*sprites)[2]=&(*textbox->chars)[1];
    (*sprites)[3]=&(*textbox->chars)[2];
    (*sprites)[4]=&(*textbox->chars)[3];
    (*sprites)[5]=&(*textbox->chars)[4];
    (*sprites)[6]=&(*textbox->chars)[5];
    (*sprites)[7]=&(*textbox->chars)[6];
    (*sprites)[8]=&(*textbox->chars)[7];
    (*sprites)[9]=&(*textbox->chars)[8];
    (*sprites)[10]=&(*textbox->chars)[9];
    (*sprites)[11]=&(*textbox->chars)[10];
    (*sprites)[12]=&(*textbox->chars)[11];
    (*sprites)[13]=&(*textbox->chars)[12];
    (*sprites)[14]=&(*textbox->chars)[13];
    (*sprites)[15]=&(*textbox->chars)[14];
    (*sprites)[16]=new Sprite;
    (*sprites)[16]->x=0.0f;
    (*sprites)[16]->y=0.5f;
    (*sprites)[16]->l=1.5f;
    (*sprites)[16]->w=0.75f;
    (*sprites)[16]->texture=ENTER_HOST_IP_ADDRESS;
    (*sprites)[16]->heapAllocated=true;
    (*sprites)[16]->visible=true;
    scene=joinGameScreen;
    updateForScene(nullptr,nullptr,0,0,textbox,scene);
    updateRenderer(usRegiments,usRegs,csRegiments,csRegs,sprites,spriteArrLength,scene,nullptr,0);
    loadingScene=false;
}
void exitGame(){
    exited=true;
}
void loadMainMenu(){
    loadingScene=true;
	if(sceneLoaded){
		unloadScene();
        setBackground(0.0f,0.0f,0.0f);
    }
    if(networkStatus!=disconnected)
        disconnect();
    networkStatus=disconnected;
	regiments=nullptr;
	infArrLength=0;
	buttons=reinterpret_cast<Button*(*)[]>(malloc(sizeof(Button*[3])));
	buttonArrLength=3;
	(*buttons)[0]=new Button(
			0.0f,
			0.5f,
			1.0f,
			0.5f,
			0.25f,
			HOST_GAME_BUTTON,
			reinterpret_cast<void*>(&hostGame),
			nullptr,
			nullptr
			);
	(*buttons)[1]=new Button(
			0.0f,
			0.0f,
			1.0f,
			0.5f,
			0.25f,
			JOIN_GAME_BUTTON,
			reinterpret_cast<void*>(&inputIp),
			nullptr,
			nullptr
			);
	(*buttons)[2]=new Button(
			0.0f,
			-0.5f,
			1.0f,
			0.5f,
			0.25f,
			EXIT_GAME_BUTTON,
			reinterpret_cast<void*>(&exitGame),
			nullptr,
			nullptr
			);
	sprites=reinterpret_cast<Sprite*(*)[]>(malloc(sizeof(Sprite*[3])));
	(*sprites)[0]=&(*buttons)[0]->sprite;
	(*sprites)[1]=&(*buttons)[1]->sprite;
	(*sprites)[2]=&(*buttons)[2]->sprite;
	spriteArrLength=3;
    scene=mainMenu;
	updateForScene(units,buttons,buttonArrLength,unitArrLength,textbox,scene);
    updateRenderer(usRegiments,usRegs,csRegiments,csRegs,sprites,spriteArrLength,scene,nullptr,0);
    loadingScene=false;
}
int main(){
    mainWindow=initRenderer(&sr);
	loadMainMenu();
	sceneLoaded=true;
    initAudio();
    auto frame=steady_clock::now();
    enet_initialize();
    while(!exited){
        frame+=milliseconds(1000/60);
        if(networkStatus==waiting&&!hasSetupHost){
            setupHost();
            hasSetupHost=true;
        }
        if(!loadingScene){
            if(networkStatus!=disconnected)
                networkStatus=listenOnPort(&playerCs,&playerCsSet);
            if(networkStatus==connected&&scene==waitingForPlayerScreen)
                loadBattlefield();
            if(networkStatus==disconnected&&scene==battlefield)
                loadMainMenu();
            for(unsigned short count=0;count<unitArrLength&&!loadingScene;++count){
                if((*units)[count]){
                    Unit*unit=(*units)[count];
                    if(unit->cs){
                        (*unit.*unit->updateState)(usUnits,usUnitsLength);
                    }else{
                        (*unit.*unit->updateState)(csUnits,csUnitsLength);
                    }
                }
            }
            if(playerCsSet){
                Unit*(*enemyUnits)[];
                Unit*(*alliedUnits)[];
                unsigned short numEnemyUnits;
                unsigned short numAlliedUnits;
                if(playerCs){
                    enemyUnits=usUnits;
                    numEnemyUnits=usUnitsLength;
                    alliedUnits=csUnits;
                    numAlliedUnits=csUnitsLength;
                }else{
                    enemyUnits=csUnits;
                    numEnemyUnits=csUnitsLength;
                    alliedUnits=usUnits;
                    numAlliedUnits=usUnitsLength;
                }
                for(unsigned short count=0;count<numEnemyUnits;++count){
                    Unit*unit=(*enemyUnits)[count];
                    if(unit&&unit->cs!=playerCs){
                        bool visible=false;
                        for(unsigned short i=0;i<numAlliedUnits&&!visible;++i){
                            if((*alliedUnits)[i])
                                visible=(*(*alliedUnits)[i].*(*alliedUnits)[i]->canSee)(unit);
                        }
                        if(unit->button->sprite.visible!=visible){
                            unit->button->sprite.visible=visible;
                            if(unit->type==infantry_regiment){
                                InfantryRegiment*reg=(InfantryRegiment*)unit;
                                reg->visible=visible;
                                reg->hunDigit.visible=visible;
                                reg->tenDigit.visible=visible;
                                reg->oneDigit.visible=visible;
                            }else if(unit->type==cavalry_regiment){
                                CavalryRegiment*reg=(CavalryRegiment*)unit;
                                reg->visible=visible;
                                reg->hunDigit.visible=visible;
                                reg->tenDigit.visible=visible;
                                reg->oneDigit.visible=visible;
                                reg->mountedSprite.visible=visible;
                            }else if(unit->type==artillery_battery){
                                ArtilleryBattery*battery=(ArtilleryBattery*)unit;
                                battery->visible=visible;
                                battery->gunDigit.visible=visible;
                                battery->limberedSprite.visible=visible;
                            }else{
                                Brigade*brigade=(Brigade*)unit;
                                brigade->thouDigit.visible=visible;
                                brigade->hunDigit.visible=visible;
                                brigade->tenDigit.visible=visible;
                                brigade->oneDigit.visible=visible;
                            }
                        }
                    }
                }
            }
            for(unsigned short count=0;count<unitArrLength&&!loadingScene;++count){
                Unit*unit=(*units)[count];
                if(unit){
                    if(unit->size<=unit->initialSize*0.7){
                        pauseRendering();
                        (*units)[count]=nullptr;
                        Button*button=unit->button;
                        Sprite*buttonSprite=&button->sprite;
                        for(unsigned short i=0;i<buttonArrLength;++i){
                            if((*buttons)[i]==button)
                                (*buttons)[i]=nullptr;
                        }
                        for(unsigned short i=0;i<spriteArrLength;++i){
                            if((*sprites)[i]==buttonSprite)
                                (*sprites)[i]=nullptr;
                        }
                        bool cs=unit->cs;
                        Unit*(*allies)[];
                        unsigned short numAllies;
                        if(cs){
                            allies=csUnits;
                            numAllies=csUnitsLength;
                        }else{
                            allies=usUnits;
                            numAllies=usUnitsLength;
                        }
                        for(unsigned short i=0;i<numAllies;++i){
                            if((*allies)[i]==unit)
                                (*allies)[i]=nullptr;
                        }
                        if(unit->type==infantry_regiment){
                            InfantryRegiment*reg=(InfantryRegiment*)unit;
                            Sprite*hunDigit=&reg->hunDigit;
                            Sprite*tenDigit=&reg->tenDigit;
                            Sprite*oneDigit=&reg->oneDigit;
                            for(unsigned short i=0;i<spriteArrLength;++i){
                                if((*sprites)[i]==hunDigit){
                                    (*sprites)[i]=nullptr;
                                }else if((*sprites)[i]==tenDigit){
                                    (*sprites)[i]=nullptr;
                                }else if((*sprites)[i]==oneDigit){
                                    (*sprites)[i]=nullptr;
                                }
                            }
                        }else if(unit->type==cavalry_regiment){
                            CavalryRegiment*reg=(CavalryRegiment*)unit;
                            Sprite*hunDigit=&reg->hunDigit;
                            Sprite*tenDigit=&reg->tenDigit;
                            Sprite*oneDigit=&reg->oneDigit;
                            Sprite*mountedSprite=&reg->mountedSprite;
                            for(unsigned short i=0;i<spriteArrLength;++i){
                                if((*sprites)[i]==hunDigit){
                                    (*sprites)[i]=nullptr;
                                }else if((*sprites)[i]==tenDigit){
                                    (*sprites)[i]=nullptr;
                                }else if((*sprites)[i]==oneDigit){
                                    (*sprites)[i]=nullptr;
                                }else if((*sprites)[i]==mountedSprite){
                                    (*sprites)[i]=nullptr;
                                }
                            }
                        }else if(unit->type==artillery_battery){
                            ArtilleryBattery*battery=(ArtilleryBattery*)unit;
                            Sprite*gunDigit=&battery->gunDigit;
                            Sprite*limberedSprite=&battery->limberedSprite;
                            for(unsigned short i=0;i<spriteArrLength;++i){
                                if((*sprites)[i]==gunDigit||(*sprites)[i]==limberedSprite)
                                    (*sprites)[i]=nullptr;
                            }
                        }
                        if(unit->type==infantry_regiment||unit->type==artillery_battery||unit->type==cavalry_regiment){
                            Regiment*reg=(Regiment*)unit;
                            alDeleteSources(1,&reg->fireSound);
                            for(unsigned short i=0;i<infArrLength;++i){
                                if((*regiments)[i]==reg)
                                    (*regiments)[i]=nullptr;
                            }
                            Regiment*(*alliedRegs)[];
                            unsigned short numAlliedRegs;
                            if(cs){
                                alliedRegs=csRegiments;
                                numAlliedRegs=csRegs;
                            }else{
                                alliedRegs=usRegiments;
                                numAlliedRegs=usRegs;
                            }
                            for(unsigned short i=0;i<numAlliedRegs;++i){
                                if((*alliedRegs)[i]==reg)
                                    (*alliedRegs)[i]=nullptr;
                            }
                            Brigade*brigade=(Brigade*)reg->brigade;
                            if(brigade){
                                unsigned short regs=brigade->regs;
                                Regiment*(*brigadeRegiments)[]=brigade->regiments;
                                for(unsigned short i=0;i<regs;++i){
                                    if((*brigadeRegiments)[i]==reg)
                                        (*brigadeRegiments)[i]=nullptr;
                                }
                            }
                        }else if(unit->type==brigade){
                            Brigade*brigade=(Brigade*)unit;
                            Sprite*thouDigit=&brigade->thouDigit;
                            Sprite*hunDigit=&brigade->hunDigit;
                            Sprite*tenDigit=&brigade->tenDigit;
                            Sprite*oneDigit=&brigade->oneDigit;
                            for(unsigned short i=0;i<spriteArrLength;++i){
                                Sprite*sprite=(*sprites)[i];
                                if(
                                        sprite==thouDigit||
                                        sprite==hunDigit||
                                        sprite==tenDigit||
                                        sprite==oneDigit
                                        )
                                    (*sprites)[i]=nullptr;
                            }
                            Regiment*(*brigadeRegs)[]=brigade->regiments;
                            unsigned short numRegs=brigade->regs;
                            for(unsigned short i=0;i<numRegs;++i){
                                Regiment*infReg=(*brigadeRegs)[i];
                                if(infReg)
                                    (*brigadeRegs)[i]->brigade=nullptr;
                            }
                            free(brigade->regiments);
                        }
                        delete unit;
                        delete button;
                        unPauseRendering();
                    }
                }
            }
            if(unitArrLength>0&&!loadingScene){
                bool usDefeated=true;
                for(unsigned short count=0;count<usRegs;++count){
                    if((*usUnits)[count])
                        usDefeated=false;
                }
                bool csDefeated=true;
                for(unsigned short count=0;count<csRegs;++count){
                    if((*csRegiments)[count])
                        csDefeated=false;
                }
                if(csDefeated){
                    loadVictoryScreen(USA_WINS);
                }else if(usDefeated){
                    loadVictoryScreen(CSA_WINS);
                }
            }
        }
        this_thread::sleep_until(frame);
    }
}
