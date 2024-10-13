#version 450
out vec4 color;
uniform bool cs;
void main(){
    if(cs){
        color=vec4(0.5f,0.5f,0.5f,1.0f);
    }else{
        color=vec4(0.0f,0.0f,0.5f,1.0f);
    }
}
