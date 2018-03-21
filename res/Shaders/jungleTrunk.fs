#version 330 core
in vec2 Texcoord;
in vec3 fragPos;
in vec3 Normal;
out vec4 fragColor;
uniform sampler2D bark;
struct DirLight{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
};
uniform DirLight dirLight;
uniform vec3 cameraPos;
vec3 CalcDirLight(DirLight light,vec3 normal,vec3 viewDir,vec3 texColor);

void main()
{
    fragColor = texture(bark,Texcoord);
    vec3 viewDir = normalize(cameraPos - fragPos);
    fragColor = vec4(CalcDirLight(dirLight,Normal,viewDir,fragColor.xyz),1.0f);
}

vec3 CalcDirLight(DirLight light,vec3 normal,vec3 viewDir,vec3 texColor)
{
    vec3 lightDir = (light.direction);
    float diff = max(dot(normal,lightDir),0.0);
    vec3 ambient = light.ambient*texColor;
    vec3 diffuse = light.diffuse*diff*texColor;
    return (ambient+diffuse);
}
