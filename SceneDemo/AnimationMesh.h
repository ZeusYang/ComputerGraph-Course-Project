#pragma once

#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>
#include "shader.h"
#include "texture.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
using std::string;
using std::vector;

namespace AnimationMesh {
	const int NUM_BONES_PER_VERTEX = 3;//定义每一个顶点最多与4个骨骼关联

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 Texcoord;
	};

	struct Texture
	{
		GLuint id;
		string type;//纹理的类型，比如diffuse纹理或是specular纹理
		string path;
	};

	struct VertexBoneData
	{
		GLuint IDs1[NUM_BONES_PER_VERTEX];//定义顶点所关联的骨骼ID
		GLuint IDs2[NUM_BONES_PER_VERTEX];
		GLfloat Weights1[NUM_BONES_PER_VERTEX];//定义该骨骼对该顶点的权重
		GLfloat Weights2[NUM_BONES_PER_VERTEX];
		VertexBoneData();
	
		void AddBoneData(unsigned int boneID, float weight);
		
	};

	struct BoneInfo
	{
		glm::mat4 BoneOffset;//定义将模型的局部空间变换到骨骼的骨骼空间的变换矩阵
		glm::mat4 FinalTransformation;//该骨骼某一时刻的动画矩阵

		BoneInfo();
	};

	class AnimationMesh
	{
	public:
		AnimationMesh();
		AnimationMesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures, vector<VertexBoneData> bones);
		void Draw(Shader *shader);
		virtual ~AnimationMesh();

	public:
		vector<Vertex> m_vertices;
		vector<GLuint> m_indices;//索引的数组
		vector<Texture> m_textures;
		vector<VertexBoneData> m_bones;//该AnimationMesh所关联的骨骼信息
	private:
		GLuint VAO, VBO, IBO;
		GLuint VBO_BONE;//Bone的数据缓冲区
		void setupAnimationMesh();
	};
}