#pragma once
#include "shader.h"
#include "texture.h"
#include <ctime>

namespace Snow {

#define PARTICLE_TYPE_LAUNCHER 0.0f
#define PARTICLE_TYPE_SHELL 1.0f
	//����ٶ�
#define MAX_VELOC glm::vec3(0.0,-3.0,0.0)
	//��С�ٶ�
#define MIN_VELOC glm::vec3(0.0,-1.0,0.0)
	//���������������
#define MAX_LAUNCH 1.0f*1000.0f
	//����������С����
#define MIN_LAUNCH 0.5f*1000.0f
	//��ʼ�㾫���С
#define INIT_SIZE 0.0f
#define MAX_SIZE 8.0f
#define MIN_SIZE 3.0f

	const int MAX_PARTICLES = 5000;//�������ӷ���ϵͳ����������
	//��ʼ��������������
	const int INIT_PARTICLES = 500;
	//��ѩ����
	const glm::vec3 center(0.0f);
	const float areaLength = 500.0f;
	const float fallHeight = 230.0f;

	struct SnowParticle
	{
		float type;
		glm::vec3 position;
		glm::vec3 velocity;
		float lifetimeMills;//����
		float size;//���ӵ㾫���С
	};

	class Snow
	{
	public:
		Snow();
		~Snow();
		void Render(float frametimeMills, glm::mat4& worldMatrix, glm::mat4 viewMatrix, glm::mat4& projectMatrix);
		void RenderShadow(float frametimeMills);
		void setLightSpace(glm::mat4& lightSpaceMatrix);
	private:
		bool InitSnow();
		void UpdateParticles(float frametimeMills);//�������ӵ�λ�õ�
		void InitRandomTexture(unsigned int size);//����1ά�������
		void RenderParticles(glm::mat4& worldMatrix, glm::mat4& viewMatrix, glm::mat4& projectMatrix);
		void GenInitLocation(SnowParticle partciles[], int nums);//���ɳ�ʼ����

		unsigned int mCurVBOIndex, mCurTransformFeedbackIndex;
		GLuint mParticleBuffers[2]; //���ӷ���ϵͳ���������㻺����
		GLuint mParticleArrays[2];
		GLuint mTransformFeedbacks[2];//���ӷ���ϵͳ��Ӧ��TransformFeedback
		GLuint mRandomTexture;//���һά����
		CTexture mSparkTexture;//Alpha����
		CTexture mStartTexture;
		float mTimer;//���ӷ������Ѿ������ʱ��
		bool mFirst;
		Shader* mUpdateShader;//�������ӵ�GPUProgram
		Shader* mRenderShader;//��Ⱦ���ӵ�GPUProgram
		Shader* mRenderShadow;
	};

}