#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in ivec3 BoneIDs1;
layout (location = 4) in vec3 Weights1;
layout (location = 5) in ivec3 BoneIDs2;
layout (location = 6) in vec3 Weights2;

out vec2 TexCoords;
out vec3 Normal;
out vec3 pos;

const int MAX_BONES = 80;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 gBones[MAX_BONES];
//uniform int gBoneCount;

void main(){
    mat4 BoneTransform = mat4(1.0f);
    BoneTransform  = gBones[BoneIDs1[0]]*Weights1[0];
    BoneTransform += gBones[BoneIDs1[1]]*Weights1[1];
    BoneTransform += gBones[BoneIDs1[2]]*Weights1[2];
    BoneTransform += gBones[BoneIDs2[0]]*Weights2[0];
    BoneTransform += gBones[BoneIDs2[1]]*Weights2[1];
    BoneTransform += gBones[BoneIDs2[2]]*Weights2[2];
    vec4 PosL = BoneTransform * vec4(position,1.0);
	gl_Position = projection * view * model * PosL;
    //Normal = (BoneTransform * vec4(normal,0.0)).xyz;
    Normal = normal;
	TexCoords = texCoords;
    pos = PosL.xyz;
}
