#version 330 core
layout (location = 0)in vec3 position;

out vec2 texcoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection*view*model*vec4(position,1.0f);
	texcoord = vec2(position.x + 12.5f,position.z + 12.5f);
	//texcoord /= 10.0f;
}