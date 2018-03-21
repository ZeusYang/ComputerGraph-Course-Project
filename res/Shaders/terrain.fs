#version 330 core
in vec2 texcoord;

out vec4 color;
in vec2 normalTex;
in vec4 FragPosLightSpace;

uniform sampler2D terrain1;
uniform sampler2D terrain2;
uniform sampler2D terrain3;
uniform sampler2D normal;

struct DirLight{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
};
uniform DirLight dirLight;

vec3 CalcDirLight(vec3 lightDir,vec3 normal,vec3 texColor,float shadow);
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

float ShadowCalculation(vec4 DepthCoord,vec3 Norm,vec3 lightDir)
{
	float visibility = 1.0;
//float bias = 0.0005;
	float bias = max(0.05 * (1.0 - dot(Norm, lightDir)), 0.0015);
	visibility -= 0.2 * (1.0 - texture(shadowMap, vec3(DepthCoord.xy + poissonDisk[0] / 8000.0,
                        (DepthCoord.z-bias) / DepthCoord.w) ));
	visibility -= 0.2 * (1.0 - texture(shadowMap, vec3(DepthCoord.xy + poissonDisk[1] / 8000.0,
                        (DepthCoord.z-bias) / DepthCoord.w) ));
	visibility -= 0.2 * (1.0 - texture(shadowMap, vec3(DepthCoord.xy + poissonDisk[2] / 8000.0,
                        (DepthCoord.z-bias) / DepthCoord.w) ));
	visibility -= 0.2 * (1.0 - texture(shadowMap, vec3(DepthCoord.xy + poissonDisk[3] / 8000.0,
                        (DepthCoord.z-bias) / DepthCoord.w) ));
	return visibility;
}

void main()
{
	color = texture(terrain1,texcoord*50);
	color = mix(texture(terrain3,texcoord*16),color,0.6f);
	vec4 tmp = texture(normal,normalTex);
	vec3 Norm = vec3(tmp.x,tmp.z,tmp.y);
	
	vec3 lightDir = normalize(vec3(300.0,300.0,300.0));
	
	float shadow = ShadowCalculation(FragPosLightSpace,Norm,lightDir);
	color = vec4(CalcDirLight(lightDir,Norm,color.xyz,shadow),1.0f);
}

vec3 CalcDirLight(vec3 lightDir,vec3 normal,vec3 texColor,float shadow)
{
    //vec3 lightDir = (light.direction);
	//lightDir = normalize(vec3(300.0,300.0,300.0));
    float diff = max(dot(normal,lightDir),0.0);
    vec3 ambient = 0.3f*texColor;
    vec3 diffuse = vec3(1.0f)*diff*texColor;
    return (ambient + shadow*diffuse);
}