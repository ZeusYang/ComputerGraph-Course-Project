#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "AnimationMesh.h"
#include "ckAssimpTool.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include  <io.h>
#include <map>

class AnimationModel
{
public:
	AnimationModel(Shader* shader, string path);
	void Draw(glm::mat4 view, glm::mat4 model, glm::mat4 projection, float timeInSeconds);
	void DrawShadow(glm::mat4 model, glm::mat4 lightSpaceMatrix, float timeInSeconds);
	int GetBones() { return m_NumBones; }
	~AnimationModel();
private:
	std::vector<AnimationMesh::AnimationMesh> meshes;
	std::string directory;
	std::map<string, AnimationMesh::Texture> texturesLoaded;
	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene, ck_aiNode* parent);
	AnimationMesh::AnimationMesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<AnimationMesh::Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
	Shader* m_pShader, *m_pShadow;
	CTexture texture;
	vector<glm::mat4> transforms;
	//动画相关
	glm::mat4 m_GlobalInverseTransform;//存放根骨骼的变换矩阵（用来将顶点的骨骼空间变换到局部空间）
	int m_NumBones;//表示模型所包含有多少个骨骼
	std::map<std::string, int> m_BoneMapping;//定义骨骼名到索引的映射
	std::vector<AnimationMesh::BoneInfo> m_BoneInfo;//定义该骨骼的信息
	ck_aiScene* m_pCkAiScene;

private:
	//动画相关
	void readNodeHierachy(float animationTime, const ck_aiNode* pNode, const glm::mat4& parentTransform);
	ck_aiAnimNode* FindNodeAnim(const ck_aiAnimation* pAnimation, string nodeName);
	void CalcInterpolatedScaling(glm::vec3& scaling, float animationTime, const ck_aiAnimNode* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& out, float animationTime, const ck_aiAnimNode* pNodeAnim);
	void CalcInterpolatedPosition(glm::vec3& rotation, float animationTime, const ck_aiAnimNode* pNodeAnim);

	GLuint FindRotationKey(float animationTime, const ck_aiAnimNode* pNodeAnim);
	GLuint FindScaleKey(float animationTime, const ck_aiAnimNode* pNodeAnim);
	GLuint FindPositionKey(float animationTime, const ck_aiAnimNode* pNodeAnim);
	void ChangeaiMatrix4x4ToMat4(glm::mat4& out, const aiMatrix4x4& mat);
	void ChangeaiMatrix3x3ToMat4(glm::mat4& out, const aiMatrix3x3& mat);
	void PreDraw(Shader* shader, float timeInSecond, vector<glm::mat4>& transforms, bool constain);
};