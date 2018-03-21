#include "flame.h"
#include "common.h"

namespace Flame {

	Flame::Flame()
	{
		glGetError();
		mCurVBOIndex = 0;
		mCurTransformFeedbackIndex = 1;
		mFirst = true;
		mTimer = 0;
		const GLchar* varyings[7] = { "Type1","Position1",
			"Velocity1","Age1",
			"Alpha1","Size1",
			"Life1"
		};//设置TransformFeedback要捕获的输出变量
		mUpdateShader = new Shader(string(ShaderPath+"Shaders/FlameUpdate.vs").c_str(),
			string(ShaderPath + "Shaders/FlameUpdate.fs").c_str(), string(ShaderPath + "Shaders/FlameUpdate.gs").c_str(), varyings, 7);
		//设置TransformFeedback缓存能够记录的顶点的数据类型

		mRenderShader = new Shader(string(ShaderPath + "Shaders/FlameRender.vs").c_str(), 
							string(ShaderPath + "Shaders/FlameRender.fs").c_str());
		mRenderShadow = new Shader(string(ShaderPath + "Shaders/FlameShadow.vs").c_str(), 
							string(ShaderPath + "Shaders/FlameShadow.fs").c_str());
		//设置随机纹理
		InitRandomTexture(580);
		mSparkTexture.loadTexture(string(ShaderPath + "textures/particle.bmp"));
		mStartTexture.loadTexture(string(ShaderPath+"textures/flame.bmp"));
		mRenderShader->use();
		mRenderShader->setInt("flameSpark", 0);
		mRenderShader->setInt("flameStart", 1);
		glm::vec3 pos(0.0, 0.0, -3.0f);
		InitFlame(pos);
	}

	void Flame::setLightSpace(glm::mat4 &lightSpace)
	{
		mRenderShadow->use();
		mRenderShadow->setMat4("lightSpaceMatrix", lightSpace);
	}

	Flame::~Flame()
	{
		delete mUpdateShader;
		delete mRenderShader;
		delete mRenderShadow;
		glDeleteTransformFeedbacks(2, mTransformFeedbacks);
		glDeleteVertexArrays(2, mParticleArrays);
		glDeleteBuffers(2, mParticleBuffers);
	}

	bool Flame::InitFlame(glm::vec3 & pos)
	{
		FlameParticle particles[MAX_PARTICLES];
		memset(particles, 0, sizeof(particles));
		particles[0].type = PARTICLE_TYPE_LAUNCHER_FL;//设置第一个粒子的类型为发射器
		particles[0].position = pos;
		particles[0].lifetimeMills = 0.0f;
		particles[0].velocity = glm::vec3(0.0f, 0.1f, 0.0f);
		GenInitLocation(particles, INIT_PARTICLES);
		glGenTransformFeedbacks(2, mTransformFeedbacks);
		glGenBuffers(2, mParticleBuffers);
		glGenVertexArrays(2, mParticleArrays);
		for (int i = 0; i < 2; i++)
		{
			glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mTransformFeedbacks[i]);
			glBindBuffer(GL_ARRAY_BUFFER, mParticleBuffers[i]);
			glBindVertexArray(mParticleArrays[i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(particles), particles, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mParticleBuffers[i]);
		}
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
		glBindVertexArray(0);
		//绑定纹理
		mUpdateShader->use();
		glBindTexture(GL_TEXTURE_1D, mRandomTexture);
		mUpdateShader->setInt("gRandomTexture", 0);
		return true;
	}

	void Flame::Render(float frametimeMills, glm::mat4& worldMatrix,
		glm::mat4 viewMatrix, glm::mat4& projectMatrix)
	{
		RenderParticles(worldMatrix, viewMatrix, projectMatrix);
		mCurVBOIndex = mCurTransformFeedbackIndex;
		mCurTransformFeedbackIndex = (mCurTransformFeedbackIndex + 1) & 0x1;
	}

	void Flame::UpdateParticles(float frametimeMills)
	{
		mUpdateShader->use();
		mUpdateShader->setFloat("gDeltaTimeMillis", frametimeMills);
		mUpdateShader->setFloat("gTime", mTimer);
		mUpdateShader->setFloat("MAX_LIFE", MAX_LIFE_FL);
		mUpdateShader->setFloat("MIN_LIFE", MIN_LIFE_FL);
		mUpdateShader->setVec3("MAX_VELOC", MAX_VELOC_FL);
		mUpdateShader->setVec3("MIN_VELOC", MIN_VELOC_FL);

		//绑定纹理
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_1D, mRandomTexture);
		//mUpdateShader->setInt("gRandomTexture",0);

		glEnable(GL_RASTERIZER_DISCARD);//我们渲染到TransformFeedback缓存中去，并不需要光栅化
		glBindVertexArray(mParticleArrays[mCurVBOIndex]);
		glBindBuffer(GL_ARRAY_BUFFER, mParticleBuffers[mCurVBOIndex]);
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mTransformFeedbacks[mCurTransformFeedbackIndex]);

		glEnableVertexAttribArray(0);//type
		glEnableVertexAttribArray(1);//position
		glEnableVertexAttribArray(2);//velocity
		glEnableVertexAttribArray(3);//lifetime
		glEnableVertexAttribArray(4);//alpha
		glEnableVertexAttribArray(5);//size
		glEnableVertexAttribArray(6);//life
		glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(FlameParticle), (void*)offsetof(FlameParticle, type));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(FlameParticle), (void*)offsetof(FlameParticle, position));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(FlameParticle), (void*)offsetof(FlameParticle, velocity));
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(FlameParticle), (void*)offsetof(FlameParticle, lifetimeMills));
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(FlameParticle), (void*)offsetof(FlameParticle, alpha));
		glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(FlameParticle), (void*)offsetof(FlameParticle, size));
		glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(FlameParticle), (void*)offsetof(FlameParticle, life));
		glBeginTransformFeedback(GL_POINTS);
		if (mFirst)
		{
			glDrawArrays(GL_POINTS, 0, INIT_PARTICLES);
			mFirst = false;
		}
		else {
			glDrawTransformFeedback(GL_POINTS, mTransformFeedbacks[mCurVBOIndex]);
		}
		glEndTransformFeedback();
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(4);
		glDisableVertexAttribArray(5);
		glDisableVertexAttribArray(6);
		glDisable(GL_RASTERIZER_DISCARD);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void Flame::RenderShadow(float frametimeMills)
	{
		mTimer += frametimeMills*1000.0f;
		UpdateParticles(frametimeMills*1000.0f);

		glEnable(GL_POINT_SPRITE);
		glEnable(GL_PROGRAM_POINT_SIZE);

		mRenderShadow->use();

		glBindVertexArray(mParticleArrays[mCurTransformFeedbackIndex]);
		glBindBuffer(GL_ARRAY_BUFFER, mParticleBuffers[mCurTransformFeedbackIndex]);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FlameParticle), (void*)offsetof(FlameParticle, position));
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(FlameParticle), (void*)offsetof(FlameParticle, size));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mSparkTexture.textureID);
		glDrawTransformFeedback(GL_POINTS, mTransformFeedbacks[mCurTransformFeedbackIndex]);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(2);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void Flame::RenderParticles(glm::mat4& worldMatrix,
		glm::mat4& viewMatrix, glm::mat4& projectMatrix)
	{
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_POINT_SPRITE);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		mRenderShader->use();
		mRenderShader->setMat4("model", worldMatrix);
		mRenderShader->setMat4("view", viewMatrix);
		mRenderShader->setMat4("projection", projectMatrix);
		glBindVertexArray(mParticleArrays[mCurTransformFeedbackIndex]);
		glBindBuffer(GL_ARRAY_BUFFER, mParticleBuffers[mCurTransformFeedbackIndex]);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FlameParticle), (void*)offsetof(FlameParticle, position));
		glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(FlameParticle), (void*)offsetof(FlameParticle, alpha));
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(FlameParticle), (void*)offsetof(FlameParticle, size));
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(FlameParticle), (void*)offsetof(FlameParticle, lifetimeMills));
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(FlameParticle), (void*)offsetof(FlameParticle, life));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mSparkTexture.textureID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mStartTexture.textureID);
		glDrawTransformFeedback(GL_POINTS, mTransformFeedbacks[mCurTransformFeedbackIndex]);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(4);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}

	void Flame::InitRandomTexture(unsigned int size)
	{
		srand(time(NULL));
		glm::vec3* pRandomData = new glm::vec3[size];
		for (int i = 0; i < size; i++)
		{
			pRandomData[i].x = float(rand()) / float(RAND_MAX);
			pRandomData[i].y = float(rand()) / float(RAND_MAX);
			pRandomData[i].z = float(rand()) / float(RAND_MAX);
		}
		glGenTextures(1, &mRandomTexture);
		glBindTexture(GL_TEXTURE_1D, mRandomTexture);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, size, 0, GL_RGB, GL_FLOAT, pRandomData);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		delete[] pRandomData;
		pRandomData = nullptr;
	}

	void Flame::GenInitLocation(FlameParticle particles[], int nums)
	{
		srand(time(NULL));
		int n = 10;
		float Adj_value = 0.05f;
		float radius = 0.90f;//火焰地区半径
		for (int x = 0; x < nums; x++) {
			glm::vec3 record(0.0f);
			for (int y = 0; y < n; y++) {//生成高斯分布的粒子，中心多，外边少
				record.x += (2.0f*float(rand()) / float(RAND_MAX) - 1.0f);
				record.z += (2.0f*float(rand()) / float(RAND_MAX) - 1.0f);
			}
			record.x *= radius;
			record.z *= radius;
			//record.y = center.y;
			record += center;
			particles[x].type = PARTICLE_TYPE_LAUNCHER_FL;
			particles[x].position = record;
			particles[x].velocity = DEL_VELOC_FL*(float(rand()) / float(RAND_MAX)) + MIN_VELOC_FL;//在最大最小速度之间随机选择
			particles[x].alpha = 1.0f;
			particles[x].size = INIT_SIZE_FL;//发射器粒子大小
											 //在最短最长寿命之间随机选择
			particles[x].lifetimeMills = (MAX_LIFE_FL - MIN_LIFE_FL)*(float(rand()) / float(RAND_MAX)) + MIN_LIFE_FL;
			float dist = sqrt(record.x*record.x + record.z*record.z);
			//中心粒子寿命要长一点
			if (dist < r)particles[x].lifetimeMills *= 0.6;
			particles[x].life = particles[x].lifetimeMills;
		}
	}
}