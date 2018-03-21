#version 330 core
layout (location = 0)in vec3 position;

out vec2 texcoord;
out vec2 normalTex;
out vec4 FragPosLightSpace;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
	gl_Position = projection*view*vec4(position,1.0f);
	texcoord = vec2(position.x+512.0f,position.z+512.0f);
	normalTex = texcoord;
	normalTex /= 1024.0f;
	texcoord /= 512.0f;
	FragPosLightSpace = lightSpaceMatrix * vec4(position, 1.0);
}