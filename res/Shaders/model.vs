#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 fragPos;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main(){
    TexCoords = texCoords;
    vec4 record = model * vec4(position,1.0f);
    fragPos = position;
    gl_Position = projection*view*record;
    Normal = normal;
    FragPosLightSpace = lightSpaceMatrix*record;
}

