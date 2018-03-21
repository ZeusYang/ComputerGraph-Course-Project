#version 330 core
layout (location = 0)in vec3 position;
layout (location = 1)in float size;

uniform mat4 lightSpaceMatrix;
void main()
{
	gl_Position = lightSpaceMatrix * vec4(position,1.0f);
    gl_PointSize = 2.0f; 
}
