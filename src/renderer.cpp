#include<GL/glew.h>
#define STB_IMAGE_IMPLEMENTATION
#include<stb/stb_image.h>
#include<iostream>
#include<sstream>
#include<fstream>
#include<vector>
#include<cmath>
#include<thread>
#include<chrono>
#include<malloc.h>
#include"renderer.hpp"
#define VERTS_PER_REG 6
#define VERTS_PER_SPRITE 5
using namespace std;
using namespace chrono;
GLFWwindow*window=nullptr;
GLuint vao;
GLuint vbo;
const float infWidth=2.0f/1000.0f;
//order of filenames MUST match the order in which textures are listed in renderer.hpp
const char*textures[]={
    "res/icons/CSInfIconUnsel.png",
    "res/icons/CSInfIconSel.png",
    "res/icons/USInfIconUnsel.png",
    "res/icons/USInfIconSel.png",
    "res/icons/CSBrigIconUnsel.png",
    "res/icons/CSBrigIconSel.png",
    "res/icons/USBrigIconUnsel.png",
    "res/icons/USBrigIconSel.png",
    "res/icons/CSArtIconUnsel.png",
    "res/icons/CSArtIconSel.png",
    "res/icons/USArtIconUnsel.png",
    "res/icons/USArtIconSel.png",
    "res/icons/CSCavIconUnsel.png",
    "res/icons/CSCavIconSel.png",
    "res/icons/USCavIconUnsel.png",
    "res/icons/USCavIconSel.png",
    "res/icons/FiringInfIconUnsel.png",
    "res/icons/FiringInfIconSel.png",
    "res/icons/FiringArtIconUnsel.png",
    "res/icons/FiringArtIconSel.png",
    "res/icons/FiringCavIconUnsel.png",
    "res/icons/FiringCavIconSel.png",
    "res/textbox.png",
    "res/cs_flag.png",
    "res/us_flag.png",
    "res/numbers/zero.png",
    "res/numbers/one.png",
    "res/numbers/two.png",
    "res/numbers/three.png",
    "res/numbers/four.png",
    "res/numbers/five.png",
    "res/numbers/six.png",
    "res/numbers/seven.png",
    "res/numbers/eight.png",
    "res/numbers/nine.png",
    "res/buttons/HostGame.png",
    "res/buttons/JoinGame.png",
    "res/buttons/ExitGame.png",
    "res/waiting.png",
    "res/dot.png",
    "res/csa_wins.png",
    "res/usa_wins.png",
    "res/could_not_connect.png",
    "res/select_side.png",
    "res/enter_host_ip_address.png",
    "res/y.png",
    "res/n.png",
};
bool initialized=false;
Regiment*(*unionRegiments)[];
Regiment*(*confederateRegiments)[];
Sprite*(*spriteArr)[];
Hill(*hillArr)[];
unsigned short usRegsCount;
unsigned short csRegsCount;
unsigned short sArrLength;
unsigned short hillArrLength;
thread*renderThread=nullptr;
bool drawing=false;
bool shouldDraw=true;
bool hasExited=false;
float backgroundColor[3];
bool backgoundChanged=false;
float cameraX=0.0f;
float cameraY=0.0f;
float zoom=1.0f;
Scene currentScene;
float screenr;
GLuint compileShader(const char*const file,const GLenum type){
    GLuint shader=glCreateShader(type);
    //load text from shader file into RAM and store address in char*
    ifstream source(file);
    if(!source)
        cout<<"ERROR: could not open file "<<file<<"\n";
    ostringstream ss;
    ss<<source.rdbuf();
    string code=ss.str();
    //create char* variable because OpenGL requires char**
    const char*const code_cstr=code.c_str();
    glShaderSource(shader,1,&code_cstr,nullptr);
    glCompileShader(shader);
    GLint maxLength;
    glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&maxLength);
    vector<GLchar>errorLog(maxLength);
    glGetShaderInfoLog(shader,maxLength,&maxLength,&errorLog[0]);
    GLint compiled;
    glGetShaderiv(shader,GL_COMPILE_STATUS,&compiled);
    if(compiled==GL_FALSE){
        cout<<"ERROR: "<<file<<" compiled with error(s):\n";
        cout<<&errorLog[0]<<"\n";
    }
    return shader;
}
GLuint buildShaderProgram(const char*vertShaderName,const char*fragShaderName){
    GLuint vertShader=compileShader(vertShaderName,GL_VERTEX_SHADER);
    GLuint fragShader=compileShader(fragShaderName,GL_FRAGMENT_SHADER);
    GLuint shaderProgram=glCreateProgram();
    glAttachShader(shaderProgram,vertShader);
    glAttachShader(shaderProgram,fragShader);
    glLinkProgram(shaderProgram);
    return shaderProgram;
}
GLuint loadTexture(const char*const file){
	int width;
	int height;
	int comp;
    const unsigned char*img=stbi_load(file,&width,&height,&comp,4);
    if(!img){
        cout<<"failed to load "<<file<<'\n';
        return 0;
    }
    GLuint texture;
    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D,texture);
	glTextureParameteri(texture,GL_TEXTURE_MAX_LEVEL,0);
	glTextureParameteri(texture,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTextureParameteri(texture,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,img);
    return texture;
} 
void renderRegs(Regiment*(*regimentArr)[],unsigned short numRegs){
    unsigned short regimentCount=0;
    for(unsigned short count=0;count<numRegs;++count){
        if((*regimentArr)[count]){
            if((*regimentArr)[count]->visible)
                ++regimentCount;
        }
    }
    Regiment*regiments[regimentCount];
    unsigned short setRegs=0;
    for(unsigned short count=0;count<numRegs;++count){
        Regiment*regiment=(*regimentArr)[count];
        if(regiment){
            if(regiment->visible){
                regiments[setRegs]=regiment;
                ++setRegs;
            }
        }
    }
    //6 is the number of corners in a pair of triangles and the numbers being added together are the vertex attributes
    unsigned short numPoints=regimentCount*6;
    unsigned short numVertices=numPoints*VERTS_PER_REG;
    GLfloat vertices[numVertices];
    unsigned short setVertices=0;
    for(unsigned short count=0;count<regimentCount;++count){
        float centerX=regiments[count]->xPosition;
        float centerY=regiments[count]->yPosition;
        float leftSide=centerX-regiments[count]->length/2.0f;
        float rightSide=centerX+regiments[count]->length/2.0f;
        float bottomSide=centerY-infWidth/2.0f;
        float topSide=centerY+infWidth/2.0f;
        float rotation=regiments[count]->rotation/180.0f*M_PI;
        vertices[setVertices]=leftSide;
        ++setVertices;
        vertices[setVertices]=topSide;
        ++setVertices;
        vertices[setVertices]=0.5f;
        ++setVertices;
        vertices[setVertices]=centerX;
        ++setVertices;
        vertices[setVertices]=centerY;
        ++setVertices;
        vertices[setVertices]=rotation;
        ++setVertices;
        vertices[setVertices]=rightSide;
        ++setVertices;
        vertices[setVertices]=topSide;
        ++setVertices;
        vertices[setVertices]=0.5f;
        ++setVertices;
        vertices[setVertices]=centerX;
        ++setVertices;
        vertices[setVertices]=centerY;
        ++setVertices;
        vertices[setVertices]=rotation;
        ++setVertices;
        vertices[setVertices]=leftSide;
        ++setVertices;
        vertices[setVertices]=bottomSide;
        ++setVertices;
        vertices[setVertices]=0.5f;
        ++setVertices;
        vertices[setVertices]=centerX;
        ++setVertices;
        vertices[setVertices]=centerY;
        ++setVertices;
        vertices[setVertices]=rotation;
        ++setVertices;
        vertices[setVertices]=rightSide;
        ++setVertices;
        vertices[setVertices]=topSide;
        ++setVertices;
        vertices[setVertices]=0.5f;
        ++setVertices;
        vertices[setVertices]=centerX;
        ++setVertices;
        vertices[setVertices]=centerY;
        ++setVertices;
        vertices[setVertices]=rotation;
        ++setVertices;
        vertices[setVertices]=leftSide;
        ++setVertices;
        vertices[setVertices]=bottomSide;
        ++setVertices;
        vertices[setVertices]=0.5f;
        ++setVertices;
        vertices[setVertices]=centerX;
        ++setVertices;
        vertices[setVertices]=centerY;
        ++setVertices;
        vertices[setVertices]=rotation;
        ++setVertices;
        vertices[setVertices]=rightSide;
        ++setVertices;
        vertices[setVertices]=bottomSide;
        ++setVertices;
        vertices[setVertices]=0.5f;
        ++setVertices;
        vertices[setVertices]=centerX;
        ++setVertices;
        vertices[setVertices]=centerY;
        ++setVertices;
        vertices[setVertices]=rotation;
        ++setVertices;
    }
    glGenBuffers(1,&vbo);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,numVertices*sizeof(GLfloat),vertices,GL_STATIC_DRAW);
    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(GLfloat),nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,6*sizeof(GLfloat),(void*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,1,GL_FLOAT,GL_FALSE,6*sizeof(GLfloat),(void*)(5*sizeof(GLfloat)));
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES,0,numPoints);
    glDeleteBuffers(1,&vbo);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDeleteVertexArrays(1,&vao);
}
void renderMain(){
    if(!glfwInit()){
        printf("FATAL ERROR: GLFW failed to initialize\n");
        abort();
    }
    GLFWmonitor*const monitor=glfwGetPrimaryMonitor();
    const GLFWvidmode*const vidMode=glfwGetVideoMode(monitor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,5);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    window=glfwCreateWindow(vidMode->width,vidMode->height,"Civil War RTS",monitor,nullptr);
    if(!window){
        printf("FATAL ERROR: failed to create window\n");
        abort();
    }
    glfwMakeContextCurrent(window);
    glewExperimental=GL_TRUE;
    glewInit();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    for(unsigned short count=0;count<MAXIMUM_VALUE-1;++count){
        loadTexture(textures[count]);
    }
    GLuint infRegShaderProgram=buildShaderProgram("shaders/infantry_regiment.vert.glsl","shaders/infantry_regiment.frag.glsl");
    GLuint spriteShaderProgram=buildShaderProgram("shaders/sprite.vert.glsl","shaders/sprite.frag.glsl");
    GLuint hillShaderProgram=buildShaderProgram("shaders/hill.vert.glsl","shaders/hill.frag.glsl");
    GLint csLoc=glGetUniformLocation(infRegShaderProgram,"cs");
    GLint widthLoc=glGetUniformLocation(infRegShaderProgram,"screenWidth");
    GLint heightLoc=glGetUniformLocation(infRegShaderProgram,"screenHeight");
    GLint infCameraPosLoc=glGetUniformLocation(infRegShaderProgram,"cameraPos");
    GLint infZoomLoc=glGetUniformLocation(infRegShaderProgram,"zoom");
    GLint spriteCameraPosLoc=glGetUniformLocation(spriteShaderProgram,"cameraPos");
    GLint spriteZoomLoc=glGetUniformLocation(spriteShaderProgram,"zoom");
    GLint screenRatioLoc=glGetUniformLocation(hillShaderProgram,"screenRatio");
    GLint hpLoc=glGetUniformLocation(hillShaderProgram,"hp");
    GLint steepLoc=glGetUniformLocation(hillShaderProgram,"steep");
    GLint hillZoomLoc=glGetUniformLocation(hillShaderProgram,"zoom");
    GLint hillCameraPosLoc=glGetUniformLocation(hillShaderProgram,"cameraPos");
    GLint hillRotationLoc=glGetUniformLocation(hillShaderProgram,"rotation");
    glUseProgram(infRegShaderProgram);
    glUniform1f(widthLoc,vidMode->width);
    glUniform1f(heightLoc,vidMode->height);
    glUseProgram(spriteShaderProgram);
    widthLoc=glGetUniformLocation(spriteShaderProgram,"screenWidth");
    heightLoc=glGetUniformLocation(spriteShaderProgram,"screenHeight");
    float height=vidMode->height;
    float width=vidMode->width;
    glUniform1f(widthLoc,width);
    glUniform1f(heightLoc,height);
    screenr=height/width;
    glUseProgram(hillShaderProgram);
    glUniform1f(screenRatioLoc,screenr);
    initialized=true;
    initInput(window,&cameraX,&cameraY,&zoom);
    auto frame=steady_clock::now();
    while(!hasExited){
        frame+=milliseconds(1000/60);
        glfwPollEvents();
        if(backgoundChanged){
            glClearColor(backgroundColor[0],backgroundColor[1],backgroundColor[2],1.0f);
            backgoundChanged=false;
        }
        if(shouldDraw){
            drawing=true;
            if(currentScene==battlefield){
                /*If the game is not on screen then the mouse position is not set. We can help
                check for this by initializing theposition to an impossible value.*/
                double mouseX=-1.0;
                double mouseY=-1.0;
                double tempMouseX=mouseX;
                double tempMouseY=mouseY;
                glfwGetCursorPos(window,&mouseX,&mouseY);
                if(mouseX>=0.0&&mouseY>=0.0&&mouseX<=vidMode->width&&mouseY<=vidMode->height){
                    if(mouseX<=5){
                        cameraX-=0.1f/zoom;
                    }else if(mouseX>=vidMode->width-5){
                        cameraX+=0.1f/zoom;
                    }
                    if(mouseY<=5){
                        cameraY+=0.1f/zoom;
                    }else if(mouseY>=vidMode->height-5){
                        cameraY-=0.1f/zoom;
                    }
                    if(tempMouseX==mouseX&&tempMouseY==mouseY){
                        printf("mouse position not set\n");
                        if(!mouseX&&!mouseY)
                            printf("mouse position is zero\n");
                    }
                }
            }else{
                cameraX=0;
                cameraY=0;
                zoom=1.0f;
            }
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            if(unionRegiments&&confederateRegiments){
                glUseProgram(infRegShaderProgram);
                glUniform2f(infCameraPosLoc,cameraX,cameraY);
                glUniform1f(infZoomLoc,zoom);
                glUniform1i(csLoc,true);
                renderRegs(confederateRegiments,csRegsCount);
                glUniform1i(csLoc,false);
                renderRegs(unionRegiments,usRegsCount);
            }
            Sprite*(*sprites)[]=spriteArr;
            unsigned short spritesLength=sArrLength;
            glUseProgram(spriteShaderProgram);
            glUniform2f(spriteCameraPosLoc,cameraX,cameraY);
            glUniform1f(spriteZoomLoc,zoom);
            for(GLuint count=1;count<MAXIMUM_VALUE;++count){
loop_start:
                glBindTexture(GL_TEXTURE_2D,count);
                unsigned short batchedSpriteCount=0;
                for(unsigned short i=0;i<spritesLength;++i){
                    Sprite*sprite=(*sprites)[i];
                    if(sprite){
                        if(sprite->texture==count&&sprite->visible)
                            ++batchedSpriteCount;
                    }
                }
                /*if(!batchedSpriteCount){
                    ++count;
                    goto loop_start;
                }*/
                Sprite*batchedSprites[batchedSpriteCount];
                unsigned short setSprites=0;
                for(unsigned short i=0;i<spritesLength;++i){
                    Sprite*sprite=(*sprites)[i];
                    if(sprite){
                        if(sprite->texture==count&&sprite->visible){
                            batchedSprites[setSprites]=sprite;
                            ++setSprites;
                        }
                    }
                }
                unsigned short numPoints=batchedSpriteCount*6;
                unsigned short numVertices=numPoints*VERTS_PER_SPRITE;
                GLfloat vertices[numVertices];
                unsigned short setVertices=0;
                for(unsigned short i=0;i<batchedSpriteCount;++i){
                    float x=batchedSprites[i]->x;
                    float y=batchedSprites[i]->y;
                    float z=batchedSprites[i]->z;
                    float halfLength=batchedSprites[i]->l/2.0f;
                    float halfWidth=batchedSprites[i]->w/2.0f;
                    float leftSide=x-halfLength;
                    float rightSide=x+halfLength;
                    float bottomSide=y-halfWidth;
                    float topSide=y+halfWidth;
                    vertices[setVertices]=leftSide;
                    ++setVertices;
                    vertices[setVertices]=topSide;
                    ++setVertices;
                    vertices[setVertices]=z;
                    ++setVertices;
                    vertices[setVertices]=0.0f;
                    ++setVertices;
                    vertices[setVertices]=1.0f;
                    ++setVertices;
                    vertices[setVertices]=rightSide;
                    ++setVertices;
                    vertices[setVertices]=topSide;
                    ++setVertices;
                    vertices[setVertices]=z;
                    ++setVertices;
                    vertices[setVertices]=1.0f;
                    ++setVertices;
                    vertices[setVertices]=1.0f;
                    ++setVertices;
                    vertices[setVertices]=leftSide;
                    ++setVertices;
                    vertices[setVertices]=bottomSide;
                    ++setVertices;
                    vertices[setVertices]=z;
                    ++setVertices;
                    vertices[setVertices]=0.0f;
                    ++setVertices;
                    vertices[setVertices]=0.0f;
                    ++setVertices;
                    vertices[setVertices]=rightSide;
                    ++setVertices;
                    vertices[setVertices]=topSide;
                    ++setVertices;
                    vertices[setVertices]=z;
                    ++setVertices;
                    vertices[setVertices]=1.0f;
                    ++setVertices;
                    vertices[setVertices]=1.0f;
                    ++setVertices;
                    vertices[setVertices]=leftSide;
                    ++setVertices;
                    vertices[setVertices]=bottomSide;
                    ++setVertices;
                    vertices[setVertices]=z;
                    ++setVertices;
                    vertices[setVertices]=0.0f;
                    ++setVertices;
                    vertices[setVertices]=0.0f;
                    ++setVertices;
                    vertices[setVertices]=rightSide;
                    ++setVertices;
                    vertices[setVertices]=bottomSide;
                    ++setVertices;
                    vertices[setVertices]=z;
                    ++setVertices;
                    vertices[setVertices]=1.0f;
                    ++setVertices;
                    vertices[setVertices]=0.0f;
                    ++setVertices;
                }
                glGenBuffers(1,&vbo);
                glBindBuffer(GL_ARRAY_BUFFER,vbo);
                glBufferData(GL_ARRAY_BUFFER,numVertices*sizeof(GLfloat),vertices,GL_STATIC_DRAW);
                glGenVertexArrays(1,&vao);
                glBindVertexArray(vao);
                glBindBuffer(GL_ARRAY_BUFFER,vbo);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5*sizeof(GLfloat),nullptr);
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5*sizeof(GLfloat),(void*)(3*sizeof(GLfloat)));
                glBindVertexArray(vao);
                glDrawArrays(GL_TRIANGLES,0,numPoints);
                glDeleteBuffers(1,&vbo);
                glDisableVertexAttribArray(0);
                glDisableVertexAttribArray(1);
                glDeleteVertexArrays(1,&vao);
            }
            if(hillArr){
                glUseProgram(hillShaderProgram);
                glUniform2f(hillCameraPosLoc,cameraX,cameraY);
                glUniform1f(hillZoomLoc,zoom);
                for(unsigned short count=0;count<hillArrLength;++count){
                    float hillX=(*hillArr)[count].x;
                    float hillY=(*hillArr)[count].y;
                    glUniform3f(hpLoc,hillX,hillY,(*hillArr)[count].height);
                    glUniform1f(hillRotationLoc,(*hillArr)[count].rotation/180.0f*M_PI);
                    glUniform2f(steepLoc,(*hillArr)[count].xSteepness,(*hillArr)[count].ySteepness);
                    Pair xVals=(*hillArr)[count].getXs(hillY,0.0f);
                    Pair yVals=(*hillArr)[count].getYs(hillX,0.0f);
                    float xSegment=(xVals.big-xVals.small)/1000.0f;
                    float ySegment=(yVals.big-yVals.small)/1000.0f;
                    Point(*vertices)[]=(Point(*)[])malloc(sizeof(Point[1000*999*2]));
                    for(float yi=0;yi<999.0f;++yi){
                        for(float xi=0;xi<1000.0f;++xi){
                            unsigned int i=yi*2000+xi*2;
                            float x=xVals.small+xi*xSegment;
                            float y=yVals.small+yi*ySegment;
                            (*vertices)[i].x=x;
                            (*vertices)[i].y=y;
                            ++i;
                            (*vertices)[i].x=x;
                            (*vertices)[i].y=y+ySegment;
                        }
                    }
                    glGenBuffers(1,&vbo);
                    glBindBuffer(GL_ARRAY_BUFFER,vbo);
                    glBufferData(GL_ARRAY_BUFFER,sizeof(Point[1000*999*2]),vertices,GL_STATIC_DRAW);
                    glGenVertexArrays(1,&vao);
                    glBindVertexArray(vao);
                    glBindBuffer(GL_ARRAY_BUFFER,vbo);
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(GLfloat),nullptr);
                    glBindVertexArray(vao);
                    glDrawArrays(GL_TRIANGLE_STRIP,0,1000*999*2*2);
                    glDeleteBuffers(1,&vbo);
                    glDisableVertexAttribArray(0);
                    glDisableVertexAttribArray(1);
                    glDeleteVertexArrays(1,&vao);
                    free(vertices);
                }
            }
            glfwSwapBuffers(window);
            drawing=false;
        }else{
            printf("Ignore this text. It exists because g++ is being stupid.\n");
        }
        this_thread::sleep_until(frame);
    }
}
GLFWwindow*initRenderer(float*sr){
    renderThread=new thread(&renderMain);
    auto time=steady_clock::now();
    while(!initialized){
        time+=milliseconds(1);
        this_thread::sleep_until(time);
    }
    *sr=screenr;
	return window;
}
void updateRenderer(
        Regiment*(*u)[],
        unsigned short ul,
        Regiment*(*c)[],
        unsigned short cl,
        Sprite*(*s)[],
        unsigned short sl,
        Scene scene,
        Hill(*hills)[],
        unsigned short numHills
        ){
    unionRegiments=u;
    confederateRegiments=c;
    spriteArr=s;
    usRegsCount=ul;
    csRegsCount=cl;
    sArrLength=sl;
    shouldDraw=true;
    currentScene=scene;
    hillArr=hills;
    hillArrLength=numHills;
}
void pauseRendering(){
    shouldDraw=false;
    auto time=steady_clock::now();
    while(drawing){
        time+=milliseconds(1);
        this_thread::sleep_until(time);
    }
}
void unPauseRendering(){
    shouldDraw=true;
}
void setBackground(float r,float g,float b){
    backgroundColor[0]=r;
    backgroundColor[1]=g;
    backgroundColor[2]=b;
    backgoundChanged=true;
}
