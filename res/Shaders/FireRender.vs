#version 330 core
layout (location = 0)in vec3 position;
layout (location = 1)in vec4 color;

out vec4 Color;
uniform mat4 view;
uniform mat4 projection;
void main()
{
	gl_PointSize = 20.0f;
	gl_Position = projection * view * vec4(position,1.0f);
    Color = color;
}
