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
	const int NUM_BONES_PER_VERTEX = 3;//����ÿһ�����������4����������

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 Texcoord;
	};

	struct Texture
	{
		GLuint id;
		string type;//��������ͣ�����diffuse�������specular����
		string path;
	};

	struct VertexBoneData
	{
		GLuint IDs1[NUM_BONES_PER_VERTEX];//���嶥���������Ĺ���ID
		GLuint IDs2[NUM_BONES_PER_VERTEX];
		GLfloat Weights1[NUM_BONES_PER_VERTEX];//����ù����Ըö����Ȩ��
		GLfloat Weights2[NUM_BONES_PER_VERTEX];
		VertexBoneData();
	
		void AddBoneData(unsigned int boneID, float weight);
		
	};

	struct BoneInfo
	{
		glm::mat4 BoneOffset;//���彫ģ�͵ľֲ��ռ�任�������Ĺ����ռ�ı任����
		glm::mat4 FinalTransformation;//�ù���ĳһʱ�̵Ķ�������

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
		vector<GLuint> m_indices;//����������
		vector<Texture> m_textures;
		vector<VertexBoneData> m_bones;//��AnimationMesh�������Ĺ�����Ϣ
	private:
		GLuint VAO, VBO, IBO;
		GLuint VBO_BONE;//Bone�����ݻ�����
		void setupAnimationMesh();
	};
}