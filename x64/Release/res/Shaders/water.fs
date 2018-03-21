#version 330 core
in vec2 texcooord;

uniform sampler2D water;
out vec4 color;

void main()
{
	color = texture(water,texcooord);
	color.a = 0.8f;
	//color = vec4(1.0,0.0,0.0,1.0f);
}