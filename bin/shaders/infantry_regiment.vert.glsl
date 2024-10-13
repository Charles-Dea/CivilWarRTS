#version 450
in vec3 inVert;
in vec2 centerPos;
in float radRotation;
uniform float screenWidth;
uniform float screenHeight;
uniform vec2 cameraPos;
uniform float zoom;
void main(){
    //rotate vertex around centerPos
    vec3 outVert=inVert;
    outVert.x=(inVert.x-centerPos.x)*cos(radRotation)-(inVert.y-centerPos.y)*sin(radRotation)+centerPos.x;
    outVert.y=(inVert.y-centerPos.y)*cos(radRotation)+(inVert.x-centerPos.x)*sin(radRotation)+centerPos.y;
    //adjust for camera position and zoom
    outVert.xy-=cameraPos;
    outVert.xy*=zoom;
    //fix strecthing that occurs due to the way opengl stores position
    float ratio=screenHeight/screenWidth;
    outVert.x*=ratio;
    gl_Position=vec4(outVert,1.0f);
}
