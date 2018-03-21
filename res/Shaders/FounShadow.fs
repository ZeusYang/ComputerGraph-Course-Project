#version 330 

uniform sampler2D water;
void main()
{
    vec4 texColor = texture(water,gl_PointCoord);
    if(texColor.r < 0.3)discard;
}
