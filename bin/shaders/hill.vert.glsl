#version 450
in vec2 inVert;
uniform vec3 hp;
uniform vec2 steep;
uniform float rotation;
uniform vec2 cameraPos;
uniform float screenRatio;
uniform float zoom;
out vec2 position;
out vec3 hillPos;
out vec2 steepness;
void main(){
    vec3 outVert;
    outVert.x=(inVert.x-hp.x)*cos(rotation)-(inVert.y-hp.y)*sin(rotation)+hp.x;
    outVert.y=(inVert.y-hp.y)*cos(rotation)+(inVert.x-hp.x)*sin(rotation)+hp.y;
    outVert.xy-=cameraPos;
    outVert.xy*=zoom;
    outVert.x*=screenRatio;
    vec2 unrotatedVert;
    unrotatedVert=inVert-cameraPos;
    unrotatedVert*=zoom;
    unrotatedVert.x*=screenRatio;
    vec3 outHillPos=vec3((hp.xy-cameraPos)*zoom,hp.z);
    outHillPos.x*=screenRatio;
    vec2 outSteep=steep/zoom;
    outSteep.x/=screenRatio;
    vec2 adjustedVals=outSteep*unrotatedVert-outSteep*outHillPos.xy;
    outVert.z=(adjustedVals.x*adjustedVals.x+adjustedVals.y*adjustedVals.y-hp.z)/10000000.0f+0.75f;
    position=unrotatedVert;
    hillPos=outHillPos;
    steepness=outSteep;
    gl_Position=vec4(outVert,1.0f);
}
