#version 330 core
layout (location = 0) in vec3 position;
layout (location = 2) in mat4 instanceMatrix;

uniform mat4 lightSpaceMatrix;

void main()
{
    gl_PointSize = 3.0f;
    gl_Position = lightSpaceMatrix*instanceMatrix*vec4(position,1.0f);
}
