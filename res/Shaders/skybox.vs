#version 330 core
layout (location = 0) in vec3 position;
out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main(){
    vec3 record = position;
	vec4 pos = projection*view*vec4(record,1.0);
	gl_Position = pos.xyww;
	TexCoords = position;
}
