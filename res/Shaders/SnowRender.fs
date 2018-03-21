#version 330 

out vec4 color;
uniform sampler2D snowflower;

void main()
{
    vec4 texColor = texture(snowflower,gl_PointCoord);
    if(texColor.r < 0.3)discard;
    color = vec4(1.0f,1.0f,1.0f,texColor.a);
}
