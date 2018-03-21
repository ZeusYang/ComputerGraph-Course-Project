#version 330 

in vec4 Color;
uniform sampler2D particle;
out vec4 color;

void main()
{
	vec4 texColor = texture(particle,gl_PointCoord);	
    if(texColor.r < 0.1f)discard;
	//color = vec4(1.0f);
    color = Color;
}
