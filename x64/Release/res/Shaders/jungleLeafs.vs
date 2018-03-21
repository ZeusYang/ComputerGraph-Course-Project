#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in mat4 instanceMatrix;

out vec2 Texcoord;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection*view*instanceMatrix*vec4(position,1.0f);    
    //Texcoord = texcoord;
    gl_PointSize = 20.0f;
}
