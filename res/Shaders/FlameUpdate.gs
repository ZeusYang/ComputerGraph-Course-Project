#version 330 core
layout (points) in;
layout (points,max_vertices = 10) out;

in float Type0[];
in vec3 Position0[];
in vec3 Velocity0[];
in float Age0[];
in float Alpha0[];
in float Size0[];
in float Life0[];

out float Type1;
out vec3 Position1;
out vec3 Velocity1;
out float Age1;
out float Alpha1;
out float Size1;
out float Life1;

uniform float gDeltaTimeMillis;//每帧时间变化量
uniform float gTime;//总的时间变化量
uniform sampler1D gRandomTexture;
uniform float MAX_LIFE;
uniform float MIN_LIFE;
uniform vec3 MAX_VELOC;
uniform vec3 MIN_VELOC;
uniform float r;

#define PARTICLE_TYPE_LAUNCHER 0.0f
#define PARTICLE_TYPE_SHELL 1.0f

vec3 GetRandomDir(float TexCoord)
{
	vec3 Dir = texture(gRandomTexture,TexCoord).xyz;
	Dir -= vec3(0.5,0.5,0.5);
	return Dir;
}

vec3 Rand(float TexCoord){//随机0-1
    vec3 ret = texture(gRandomTexture,TexCoord).xyz;
    return ret; 
}

void main()
{
    float Age = Age0[0] - gDeltaTimeMillis;
	if(Type0[0] == PARTICLE_TYPE_LAUNCHER){//火焰发射粒子
        if(Age <= 0 ){
            //发射第二级粒子
            Type1 = PARTICLE_TYPE_SHELL;
            Position1 = Position0[0];
            //与初始发射器一样
            Velocity1 = (MAX_VELOC-MIN_VELOC)*Rand(Age0[0]).x+MIN_VELOC;
            Age1 = (MAX_LIFE-MIN_LIFE)*Rand(Age0[0]).y + MIN_LIFE;

            float dist = sqrt(Position1.x*Position1.x + Position1.z*Position1.z);
            //if(dist <= r)Age1 *= 1.5f;
            Age1 *= (1.0f+(r/dist));
            Life1 = Age1;
            Alpha1 = Alpha0[0];
            Size1 = Size0[0];
            EmitVertex();
            EndPrimitive();
            Age = (MAX_LIFE-MIN_LIFE)*Rand(Age0[0]).z + MIN_LIFE;
        }
        Type1 = PARTICLE_TYPE_LAUNCHER;
        Position1 = Position0[0];
        Velocity1 = Velocity0[0];
        Age1 = Age;
        Alpha1 = Alpha0[0];
        Size1 = Size0[0];
        Life1 = Life0[0];
        EmitVertex();
        EndPrimitive();
      }
    else{
        if(Age >= 0){
            float DeltaTimeSecs = gDeltaTimeMillis/1000.0f;
            vec3 DeltaP = Velocity0[0]*DeltaTimeSecs;
            Type1 = PARTICLE_TYPE_SHELL;
            Position1 = Position0[0] + DeltaP;
            Velocity1 = Velocity0[0];
            Age1 = Age;
            Life1 = Life0[0];
            //在粒子生命周期中，一开始比较小，后来增大，然后又减小
            //以下用当前剩余寿命和全部寿命设置大小和alpha,实际上曲线是呈现正太分布，中间大，两边小
            float factor = 1.0f/((Age/1000.0f - Life1/2000.0f)*(Age/1000.0f - Life1/2000.0f)+1);
            Alpha1 = factor;
            Size1 = 20.0*factor;
            EmitVertex();
            EndPrimitive();
        }
    }
}
