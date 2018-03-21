#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 TexCoord;
out vec3 fragPos;
uniform mat4 view;
uniform mat4 projection;
out vec4 FragPosLightSpace;
uniform mat4 lightSpaceMatrix;

void main(){
    gl_Position = projection*view*vec4(position,1.0f);	
    TexCoord = texCoord;
    fragPos = position;
    FragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
}
