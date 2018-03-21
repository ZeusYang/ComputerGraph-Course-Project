#include "fountain.h"

namespace Fountain {

	Fountain::Fountain()
	{
		mCurVBOIndex = 0;
		mCurTransformFeedbackIndex = 1;
		mFirst = true;
		mTimer = 0;
		const GLchar* varyings[4] = { "Type1","Position1",
			"Velocity1","Age1"
		};//����TransformFeedbackҪ������������
		mUpdateShader = new Shader(string(ShaderPath + "Shaders/FountainUpdate.vs").c_str(),
			string(ShaderPath + "Shaders/FountainUpdate.fs").c_str(), 
			string(ShaderPath + "Shaders/FountainUpdate.gs").c_str(), varyings, 4);
		//����TransformFeedback�����ܹ���¼�Ķ������������

		mRenderShader = new Shader(string(ShaderPath + "Shaders/FountainRender.vs").c_str(), 
									string(ShaderPath + "Shaders/FountainRender.fs").c_str());
		mRenderShadow = new Shader(string(ShaderPath + "Shaders/FounShadow.vs").c_str(), 
									string(ShaderPath + "Shaders/FounShadow.fs").c_str());
		//�����������
		InitRandomTexture(512);
		mSparkTexture.loadTexture(ShaderPath + "textures/water.bmp");
		mRenderShader->use();
		mRenderShader->setInt("water", 0);
		InitFountain();
	}

	void Fountain::setLightSpace(glm::mat4& lightSpaceMatrix)
	{
		mRenderShadow->use();
		mRenderShadow->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glUseProgram(0);
	}

	Fountain::~Fountain()
	{
		delete mUpdateShader;
		delete mRenderShader;
		delete mRenderShadow;
		glDeleteTransformFeedbacks(2, mTransformFeedbacks);
		glDeleteVertexArrays(2, mParticleArrays);
		glDeleteBuffers(2, mParticleBuffers);
	}

	bool Fountain::InitFountain()
	{
		WaterParticle particles[MAX_PARTICLES];
		memset(particles, 0, sizeof(particles));
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
		//������
		mUpdateShader->use();
		glBindTexture(GL_TEXTURE_1D, mRandomTexture);
		mUpdateShader->setInt("gRandomTexture", 0);
		return true;
	}

	void Fountain::Render(float frametimeMills, glm::mat4& worldMatrix,
		glm::mat4 viewMatrix, glm::mat4& projectMatrix)
	{
		RenderParticles(worldMatrix, viewMatrix, projectMatrix);
		mCurVBOIndex = mCurTransformFeedbackIndex;
		mCurTransformFeedbackIndex = (mCurTransformFeedbackIndex + 1) & 0x1;
	}

	void Fountain::UpdateParticles(float frametimeMills)
	{
		mUpdateShader->use();
		mUpdateShader->setFloat("gDeltaTimeMillis", frametimeMills);
		//mUpdateShader->setFloat("MAX_SIZE",MAX_SIZE_F);
		mUpdateShader->setFloat("angle", ANGLE);
		mUpdateShader->setFloat("R", radius);
		mUpdateShader->setFloat("MAX_LAUNCH", MAX_LAUNCH_F);
		mUpdateShader->setFloat("MIN_LAUNCH", MIN_LAUNCH_F);
		mUpdateShader->setVec3("NORMAL", glm::vec3(0, 1, 0));
		mUpdateShader->setVec3("center", center);

		//������
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_1D, mRandomTexture);

		glEnable(GL_RASTERIZER_DISCARD);//������Ⱦ��TransformFeedback������ȥ��������Ҫ��դ��
		glBindVertexArray(mParticleArrays[mCurVBOIndex]);
		glBindBuffer(GL_ARRAY_BUFFER, mParticleBuffers[mCurVBOIndex]);
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mTransformFeedbacks[mCurTransformFeedbackIndex]);

		glEnableVertexAttribArray(0);//type
		glEnableVertexAttribArray(1);//position
		glEnableVertexAttribArray(2);//velocity
		glEnableVertexAttribArray(3);//lifetime
		glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(WaterParticle), (void*)offsetof(WaterParticle, type));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(WaterParticle), (void*)offsetof(WaterParticle, position));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(WaterParticle), (void*)offsetof(WaterParticle, velocity));
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(WaterParticle), (void*)offsetof(WaterParticle, lifetimeMills));
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
		glDisable(GL_RASTERIZER_DISCARD);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void Fountain::RenderShadow(float frametimeMills)
	{
		mTimer += frametimeMills*1000.0f;
		UpdateParticles(frametimeMills*1000.0f);

		glEnable(GL_POINT_SPRITE);
		glEnable(GL_PROGRAM_POINT_SIZE);

		mRenderShadow->use();
		glBindVertexArray(mParticleArrays[mCurTransformFeedbackIndex]);
		glBindBuffer(GL_ARRAY_BUFFER, mParticleBuffers[mCurTransformFeedbackIndex]);
		glEnableVertexAttribArray(0);
		//glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(WaterParticle),
			(void*)offsetof(WaterParticle, position));
		//glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(WaterParticle), 
		//                       (void*)offsetof(WaterParticle,size));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mSparkTexture.textureID);
		glDrawTransformFeedback(GL_POINTS, mTransformFeedbacks[mCurTransformFeedbackIndex]);
		//glDisableVertexAttribArray(0);
		//glDisableVertexAttribArray(1);
		glDisable(GL_BLEND);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void Fountain::RenderParticles(glm::mat4& worldMatrix,
		glm::mat4& viewMatrix, glm::mat4& projectMatrix)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_POINT_SPRITE);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		mRenderShader->use();
		mRenderShader->setMat4("view", viewMatrix);
		mRenderShader->setMat4("projection", projectMatrix);
		glBindVertexArray(mParticleArrays[mCurTransformFeedbackIndex]);
		glBindBuffer(GL_ARRAY_BUFFER, mParticleBuffers[mCurTransformFeedbackIndex]);
		glEnableVertexAttribArray(0);
		//glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(WaterParticle),
			(void*)offsetof(WaterParticle, position));
		//glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(WaterParticle), 
		//                       (void*)offsetof(WaterParticle,size));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mSparkTexture.textureID);
		glDrawTransformFeedback(GL_POINTS, mTransformFeedbacks[mCurTransformFeedbackIndex]);
		glDisableVertexAttribArray(0);
		//glDisableVertexAttribArray(1);
		glDisable(GL_BLEND);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glDisable(GL_CULL_FACE);
	}

	void Fountain::InitRandomTexture(unsigned int size)
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

	void Fountain::GenInitLocation(WaterParticle particles[], int nums) {
		srand(time(NULL));
		for (int x = 0; x < nums; x++) {
			glm::vec3 record(0.0f);
			record.x = (2.0f*float(rand()) / float(RAND_MAX) - 1.0f)*radius;
			record.z = (2.0f*float(rand()) / float(RAND_MAX) - 1.0f)*radius;
			while (sqrt(record.x*record.x + record.z*record.z) > radius) {
				record.x = (2.0f*float(rand()) / float(RAND_MAX) - 1.0f)*radius;
				record.z = (2.0f*float(rand()) / float(RAND_MAX) - 1.0f)*radius;
			}
			record += center;
			particles[x].type = PARTICLE_TYPE_LAUNCHER;
			particles[x].position = record;
			particles[x].velocity = glm::vec3(0.0f);
			//particles[x].size = INIT_SIZE_F;//���������Ӵ�С
			particles[x].lifetimeMills = (MAX_LAUNCH_F - MIN_LAUNCH_F)*(float(rand()) / float(RAND_MAX)) + MIN_LAUNCH_F;
		}
	}
}