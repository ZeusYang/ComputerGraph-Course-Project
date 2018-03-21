#version 330 core
in vec2 TexCoord;
in vec3 fragPos;
in vec4 FragPosLightSpace;
out vec4 color;
uniform sampler2D floor[2];
struct DirLight{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
};
uniform DirLight dirLight;
uniform vec3 cameraPos;
vec3 CalcDirLight(DirLight light,vec3 normal,vec3 viewDir,vec3 texColor,float shadow);
uniform vec3 Normal;

uniform sampler2DShadow shadowMap;

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

float ShadowCalculation(vec4 DepthCoord)
{
	float visibility = 1.0;
    float bias = 0.0;
	visibility -= 0.2 * (1.0 - texture(shadowMap, vec3(DepthCoord.xy + poissonDisk[0] / 8000.0,
                        (DepthCoord.z) / DepthCoord.w) ));
	visibility -= 0.2 * (1.0 - texture(shadowMap, vec3(DepthCoord.xy + poissonDisk[1] / 8000.0,
                        (DepthCoord.z) / DepthCoord.w) ));
	visibility -= 0.2 * (1.0 - texture(shadowMap, vec3(DepthCoord.xy + poissonDisk[2] / 8000.0,
                        (DepthCoord.z) / DepthCoord.w) ));
	visibility -= 0.2 * (1.0 - texture(shadowMap, vec3(DepthCoord.xy + poissonDisk[3] / 8000.0,
                        (DepthCoord.z) / DepthCoord.w) ));
	return visibility;
}


void main(){
    float units = 10.0f;
    color = texture(floor[0],TexCoord*1.0f*units);
    color = mix(color,texture(floor[1],TexCoord*units),0.5);
    vec3 viewDir = normalize(cameraPos - fragPos);
    float shadow = ShadowCalculation(FragPosLightSpace);
    color = vec4(CalcDirLight(dirLight,Normal,viewDir,color.xyz,shadow),1.0f);
}

vec3 CalcDirLight(DirLight light,vec3 normal,vec3 viewDir,vec3 texColor,float shadow)
{
    vec3 lightDir = (light.direction);
    float diff = max(dot(normal,lightDir),0.0);
    vec3 ambient = light.ambient*texColor;
    vec3 diffuse = vec3(1.0f)*diff*texColor;
    return shadow*(ambient+diffuse);
}
