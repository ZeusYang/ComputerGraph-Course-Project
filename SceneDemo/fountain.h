#pragma once
#include "shader.h"
#include "texture.h"
#include <ctime>
#include "common.h"
using namespace std;

namespace Fountain {

	#define PARTICLE_TYPE_LAUNCHER 0.0f
	#define PARTICLE_TYPE_SHELL 1.0f
		//���������������
	#define MAX_LAUNCH_F 1.0f*1000.0f
		//����������С����
	#define MIN_LAUNCH_F 0.3f*1000.0f

	const float ANGLE = 120.0f;
	const int MAX_PARTICLES = 9000;//�������ӷ���ϵͳ����������
	//��ʼ��������������
	const int INIT_PARTICLES = 1500;
	//��Ȫ����
	const glm::vec3 center(-300, 28, -170.0f);
	const float radius = 0.01f;

	struct WaterParticle
	{
		float type;
		glm::vec3 position;
		glm::vec3 velocity;
		float lifetimeMills;//����
							//float size;//���ӵ㾫���С
	};

	class Fountain
	{
	public:
		Fountain();
		~Fountain();
		void Render(float frametimeMills, glm::mat4& worldMatrix, glm::mat4 viewMatrix, glm::mat4& projectMatrix);
		void RenderShadow(float frametimeMills);
		void setLightSpace(glm::mat4& lightSpaceMatrix);
	private:
		bool InitFountain();
		void UpdateParticles(float frametimeMills);//�������ӵ�λ�õ�
		void InitRandomTexture(unsigned int size);//����1ά�������
		void RenderParticles(glm::mat4& worldMatrix, glm::mat4& viewMatrix, glm::mat4& projectMatrix);
		void GenInitLocation(WaterParticle partciles[], int nums);//���ɳ�ʼ����

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