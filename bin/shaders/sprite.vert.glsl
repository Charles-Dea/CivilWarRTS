#version 450
in vec3 inVert;
in vec2 texCoord;
uniform float screenWidth;
uniform float screenHeight;
uniform vec2 cameraPos;
uniform float zoom;
out vec2 texCoords;
void main(){
    float ratio=screenHeight/screenWidth;
    vec3 outVert=inVert;
    outVert.xy-=cameraPos;
    outVert*=zoom;
    outVert.x*=ratio;
    gl_Position=vec4(outVert,1.0f);
    texCoords=texCoord;
}
