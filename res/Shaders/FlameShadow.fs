#version 330 

uniform sampler2D flameSpark;

void main()
{
    vec4 texColor;
    texColor = texture(flameSpark,gl_PointCoord);
    if(texColor.r < 0.1f)discard;
}
