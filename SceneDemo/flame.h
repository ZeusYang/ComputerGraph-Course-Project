#pragma once
#include "shader.h"
#include "texture.h"
#include <ctime>
using namespace std;

namespace Flame {

	#define PARTICLE_TYPE_LAUNCHER_FL 0.0f
	#define PARTICLE_TYPE_SHELL_FL 1.0f
		//����ٶ�
	#define MAX_VELOC_FL glm::vec3(0.0,8.0,0.0)
		//��С�ٶ�
	#define MIN_VELOC_FL glm::vec3(0.0,6.0,0.0)
		//�����С�ٶȲ��
	#define DEL_VELOC_FL glm::vec3(0.0,2.0,0.0)
		//���������
	#define MAX_LIFE_FL 2.0f*1000
		//�����������
	#define MIN_LIFE_FL 1.0f*1000
		//��ʼ�㾫���С_FL
	#define INIT_SIZE_FL 10.0f;

	const int MAX_PARTICLES = 10000;//�������ӷ���ϵͳ����������
	//��ʼ��������������
	const int INIT_PARTICLES = 5000;
	//��������
	const glm::vec3 center(-162, 8, -167.0);
	const float r = 5.6f;

	struct FlameParticle
	{
		float type;
		glm::vec3 position;
		glm::vec3 velocity;
		float lifetimeMills;//����
		float alpha;//alphaͨ��
		float size;//���ӵ㾫���С
		float life;//����
	};

	class Flame
	{
	public:
		Flame();
		~Flame();
		void Render(float frametimeMills, glm::mat4& worldMatrix, glm::mat4 viewMatrix, glm::mat4& projectMatrix);
		void setLightSpace(glm::mat4 &lightSpace);
		void RenderShadow(float frametimeMills);
	private:
		bool InitFlame(glm::vec3& pos);
		void UpdateParticles(float frametimeMills);//�������ӵ�λ�õ�
		void InitRandomTexture(unsigned int size);//����1ά�������
		void RenderParticles(glm::mat4& worldMatrix, glm::mat4& viewMatrix, glm::mat4& projectMatrix);
		void GenInitLocation(FlameParticle partciles[], int nums);//���ɳ�ʼ����

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