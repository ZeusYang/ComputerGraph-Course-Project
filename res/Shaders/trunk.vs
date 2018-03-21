#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 normal;

out vec2 Texcoord;
out vec3 fragPos;
out vec3 Normal;
out vec4 FragPosLightSpace;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
    gl_Position = projection*view*model*vec4(position,1.0f);
    Texcoord = texcoord;
    fragPos = position;
    Normal = normal;
    FragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
}
