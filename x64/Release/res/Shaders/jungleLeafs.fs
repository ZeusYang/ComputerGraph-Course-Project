#version 330 core

in vec2 Texcoord;
out vec4 fragColor;
uniform sampler2D leaf;

void main()
{
    fragColor = texture(leaf,gl_PointCoord);
    if(fragColor.a < 0.3)discard;
    fragColor = vec4(0.961,0.971,0.902,fragColor.a);
}
