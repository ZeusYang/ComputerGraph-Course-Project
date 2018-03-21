#pragma once
#include "shader.h"
#include "texture.h"
#include <ctime>
using namespace std;

namespace Flame {

	#define PARTICLE_TYPE_LAUNCHER_FL 0.0f
	#define PARTICLE_TYPE_SHELL_FL 1.0f
		//最大速度
	#define MAX_VELOC_FL glm::vec3(0.0,8.0,0.0)
		//最小速度
	#define MIN_VELOC_FL glm::vec3(0.0,6.0,0.0)
		//最大最小速度差距
	#define DEL_VELOC_FL glm::vec3(0.0,2.0,0.0)
		//最长生命周期
	#define MAX_LIFE_FL 2.0f*1000
		//最短生命周期
	#define MIN_LIFE_FL 1.0f*1000
		//初始点精灵大小_FL
	#define INIT_SIZE_FL 10.0f;

	const int MAX_PARTICLES = 10000;//定义粒子发射系统最大的粒子数
	//初始发射器例子数量
	const int INIT_PARTICLES = 5000;
	//火焰中心
	const glm::vec3 center(-162, 8, -167.0);
	const float r = 5.6f;

	struct FlameParticle
	{
		float type;
		glm::vec3 position;
		glm::vec3 velocity;
		float lifetimeMills;//年龄
		float alpha;//alpha通道
		float size;//粒子点精灵大小
		float life;//寿命
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
		void UpdateParticles(float frametimeMills);//更新粒子的位置等
		void InitRandomTexture(unsigned int size);//生成1维随机纹理
		void RenderParticles(glm::mat4& worldMatrix, glm::mat4& viewMatrix, glm::mat4& projectMatrix);
		void GenInitLocation(FlameParticle partciles[], int nums);//生成初始粒子

		unsigned int mCurVBOIndex, mCurTransformFeedbackIndex;
		GLuint mParticleBuffers[2]; //粒子发射系统的两个顶点缓存区
		GLuint mParticleArrays[2];
		GLuint mTransformFeedbacks[2];//粒子发射系统对应的TransformFeedback
		GLuint mRandomTexture;//随机一维纹理
		CTexture mSparkTexture;//Alpha纹理
		CTexture mStartTexture;
		float mTimer;//粒子发射器已经发射的时间
		bool mFirst;
		Shader* mUpdateShader;//更新粒子的GPUProgram
		Shader* mRenderShader;//渲染粒子的GPUProgram
		Shader* mRenderShadow;
	};

}