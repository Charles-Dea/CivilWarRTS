#version 450
in vec2 position;
in vec3 hillPos;
in vec2 steepness;
out vec4 color;
void main(){
    vec2 adjustedCoords;
    adjustedCoords.x=steepness.x*position.x-steepness.x*hillPos.x;
    adjustedCoords.y=steepness.y*position.y-steepness.y*hillPos.y;
    float zf=(-1.0f*(adjustedCoords.x*adjustedCoords.x)-(adjustedCoords.y*adjustedCoords.y)+hillPos.z)*100;
    int z;
    bool zSet;
    if(isnan(zf)==false&&isinf(zf)==false){
        z=int(zf);
        zSet=true;
    }else{
        zSet=false;
    }
    if(zSet){
        if(z%20==0&&z>=0){
            color=vec4(1.0f,1.0f,1.0f,1.0f);
        }else{
            color=vec4(0.0f,0.5f,0.0f,1.0f);
        }
    }
}
