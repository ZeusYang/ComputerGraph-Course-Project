#include "AnimationMesh.h"

namespace AnimationMesh {

	VertexBoneData::VertexBoneData()
	{
		memset(Weights1, 0, sizeof(Weights1));
		memset(Weights2, 0, sizeof(Weights2));
		memset(IDs1, 0, sizeof(IDs1));
		memset(IDs2, 0, sizeof(IDs2));
	}
	void VertexBoneData::AddBoneData(unsigned int boneID, float weight)
	{
		for (int i = 0; i < NUM_BONES_PER_VERTEX; i++)
		{
			if (Weights1[i] == 0.0f)
			{
				IDs1[i] = boneID;
				Weights1[i] = weight;
				return;
			}
		}

		for (int i = 0; i < NUM_BONES_PER_VERTEX; i++)
		{
			if (Weights2[i] == 0.0f)
			{
				IDs2[i] = boneID;
				Weights2[i] = weight;
				return;
			}
		}
		//到这里就应该要抛异常(因为顶点关联的骨骼超过的我们定义的最大的范围)
		assert(0);
	}

	BoneInfo::BoneInfo() {
		BoneOffset = glm::mat4(1.0f);
		FinalTransformation = glm::mat4(1.0f);
	}

	AnimationMesh::AnimationMesh()
	{
	}

	AnimationMesh::AnimationMesh(vector<Vertex>vertices, vector<GLuint> indices, vector<Texture> textures, vector<VertexBoneData> bones)
	{
		m_vertices = vertices;
		m_indices = indices;
		m_textures = textures;
		m_bones = bones;
		setupAnimationMesh();
	}

	AnimationMesh::~AnimationMesh()
	{
	}

	void AnimationMesh::setupAnimationMesh()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &IBO);
		glGenBuffers(1, &VBO_BONE);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), &m_indices[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			(GLvoid*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			(GLvoid*)(offsetof(Vertex, Texcoord)));
		//上传Bone相关数据
		if (m_bones.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, VBO_BONE);
			glBufferData(GL_ARRAY_BUFFER, sizeof(m_bones[0])*m_bones.size(), &m_bones[0], GL_STATIC_DRAW);//将骨骼数据上传
			glEnableVertexAttribArray(3);
			glVertexAttribIPointer(3, NUM_BONES_PER_VERTEX, GL_INT, sizeof(VertexBoneData),
				(void*)offsetof(VertexBoneData, IDs1));
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, NUM_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData),
				(void*)offsetof(VertexBoneData, Weights1));
			glEnableVertexAttribArray(5);
			glVertexAttribIPointer(5, NUM_BONES_PER_VERTEX, GL_INT, sizeof(VertexBoneData),
				(void*)offsetof(VertexBoneData, IDs2));
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, NUM_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData),
				(void*)offsetof(VertexBoneData, Weights2));

		}

		glBindVertexArray(0);
	}


	void AnimationMesh::Draw(Shader * shader)
	{
		GLuint diffuseIndex = 1;
		GLuint specularIndex = 1;
		for (int i = 0; i < m_textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			std::stringstream ss;
			string name = m_textures[i].type;
			if (name == "texture_diffuse")
				ss << diffuseIndex++;
			if (name == "texture_specular")
				ss << specularIndex++;
			string number = ss.str();
			shader->setInt(name + number, i);
			glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
		}
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	unsigned int CreateTextureFromFile(const char *path)
	{
		string filename = string(path);
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		if (data) {
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}
}