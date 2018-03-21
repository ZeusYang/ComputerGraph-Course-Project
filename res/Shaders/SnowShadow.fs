#version 330 

uniform sampler2D snowflower;

void main()
{
    vec4 texColor = texture(snowflower,gl_PointCoord);
    if(texColor.r < 0.3)discard;
}
