#version 330 core
in vec2 TexCoord;
out vec4 color;

uniform sampler2D shadowMap;

void main(){
    color = vec4(vec3(texture(shadowMap,TexCoord).r),1.0f);
}

