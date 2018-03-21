#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 Normal;
in vec3 pos;
uniform sampler2D texture_diffuse1;
uniform sampler2D tex;

struct DirLight{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
};

vec3 CalcDirLight(DirLight light,vec3 normal,vec3 texColor);
uniform DirLight dirLight;

void main()
{    
    FragColor = texture(tex,TexCoords);
    FragColor = vec4(CalcDirLight(dirLight,Normal,FragColor.xyz),1.0f);
    //FragColor = vec4(vec3(TexCoords,1.0f),1.0f);
    //vec4 fogColor = vec4(0.3,0.3,0.3,1.0);
    //FragColor = mix(fogColor,FragColor,fogFactor);
}

vec3 CalcDirLight(DirLight light,vec3 normal,vec3 texColor)
{
    vec3 lightDir = (light.direction);
    float diff = max(dot(normal,lightDir),0.0);
    vec3 ambient = light.ambient*texColor;
    vec3 diffuse = light.diffuse*diff*texColor;
    return (ambient+diffuse);
}
