#version 330 core
layout (points) in;
layout (points,max_vertices = 128) out;

in float Type0[];
in vec3 Position0[];
in vec3 Velocity0[];
in float Age0[];
in float Tag0[];
in vec4 Color0[];

out float Type1;
out vec3 Position1;
out vec3 Velocity1;
out float Age1;
out float Tag1;
out vec4 Color1;

uniform float gDeltaTimeMillis;//每帧时间变化量
uniform float gTime;//总的时间变化量
uniform sampler1D gRandomTexture;
uniform float gLauncherLifetime;//生命周期
uniform float gShellLifetime;
uniform float gSecondaryShellLifetime;

#define PARTICLE_TYPE_LAUNCHER 0.0f
#define PARTICLE_TYPE_SHELL 1.0f
#define PARTICLE_TYPE_SECONDARY_SHELL 2.0f

vec3 GetRandomDir(float TexCoord);
vec3 Rand(float TexCoord);
vec3 ColorRand(float TexCoord);

void main()
{
	float Age = Age0[0] + gDeltaTimeMillis;
    float SpeedRate = 100.0f;
   	if(Type0[0] == PARTICLE_TYPE_LAUNCHER){
		if(Age >= gLauncherLifetime){
			Type1 = PARTICLE_TYPE_SHELL;
			Position1 = Position0[0];
			vec3 Dir = GetRandomDir((gTime+Age0[0])/1000.0f);
			Dir.y = max(Dir.y,0.5);
			Velocity1 = normalize(Dir)*SpeedRate;
			Age1 = 0.0f;
            Tag1 = (1)*200;
            Color1 = vec4(ColorRand((gTime+Age0[0])/1000.0f),1.0f);
			EmitVertex();
			EndPrimitive();
			Age = 0.0f;
		}	
		Type1 = PARTICLE_TYPE_LAUNCHER;
		Position1 = Position0[0];
		Velocity1 = Velocity0[0];
		Age1 = Age;
        Tag1 = Tag0[0];
		EmitVertex();
		EndPrimitive();
	}
	else{
		float DeltaTimeSecs = gDeltaTimeMillis/1000.0f;
		vec3 DeltaP = Velocity0[0] * DeltaTimeSecs;
		vec3 DeltaV = DeltaTimeSecs*vec3(0.0,-1.81,0.0);
		if(Type0[0] == PARTICLE_TYPE_SHELL){
			if(Age < gShellLifetime){
				Type1 = PARTICLE_TYPE_SHELL;
				Position1 = Position0[0] + DeltaP;
				Velocity1 = Velocity0[0] + DeltaV;
				Age1 = Age;
                Tag1 = Tag0[0];
                Color1 = Color0[0];
				EmitVertex();
				EndPrimitive();
            }	
			else{
				for(int i = 0;i < 128;i ++){
					Type1 = PARTICLE_TYPE_SECONDARY_SHELL;
					Position1 = Position0[0];
					vec3 Dir = Rand((gTime + i+Tag0[0])/1000.0);
                    float zv = sqrt(1-Dir.z*Dir.z);
                    float sita = 3.14159*Dir.x;
                    Velocity1 = normalize(vec3(10.0f*zv*cos(sita),10.0f*zv*sin(sita),10.0f*Dir.z))*SpeedRate*0.5;
					Age1 = 0.0f;
                    Tag1 = 0;
                    Color1 = Color0[0];
					EmitVertex();
					EndPrimitive();
				}	
			}
		}
		else{
			if(Age < gSecondaryShellLifetime){
				Type1 = PARTICLE_TYPE_SECONDARY_SHELL;
				Position1 = Position0[0] + DeltaP;
				Velocity1 = Velocity0[0] + DeltaV;
				Age1 = Age;
                Tag1 = 0;
                Color1 = Color0[0];
                Color1.w = Age1/gSecondaryShellLifetime;
				EmitVertex();
				EndPrimitive();
			}	
		}
	}
}

vec3 GetRandomDir(float TexCoord)
{
	vec3 Dir = texture(gRandomTexture,TexCoord).xyz;
	Dir -= vec3(0.5,0.5,0.5);
	return Dir;
}

vec3 Rand(float TexCoord)
{
    vec3 Dir = texture(gRandomTexture,TexCoord).xyz;
    Dir -= vec3(0.5,0.5,0.5);
    return Dir*2;
}

vec3 ColorRand(float TexCoord)
{
    vec3 Dir = texture(gRandomTexture,TexCoord).xyz;
    Dir.z *= 0.2;
    Dir.y *= 0.5;
    return Dir;
}
