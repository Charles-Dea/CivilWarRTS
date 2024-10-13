#version 450
in vec2 texCoords;
uniform sampler2D inTexture;
out vec4 color;
void main(){
	vec2 outTexCoords=texCoords;
	outTexCoords.y*=-1;
    color=texture(inTexture,outTexCoords).rgba;
}
