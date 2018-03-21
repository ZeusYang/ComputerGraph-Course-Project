#include "SceneController.h"

Scene::Scene(float time)
{
	skybox = new Skybox;
	//floor = new Floor;
	//debug = new Debug;
	fractTree = new FractalSystem;
	snow = new Snow::Snow;
	//fireWork = new FireWork::FireWork;
	flame = new Flame::Flame;
	fountain = new Fountain::Fountain;
	terrain = new Terrain(string(ShaderPath + "Textures/Height.png"));
	water = new Water(time);
	std::cout << "terrain loaded over!" << std::endl;
	castle = new Model(ShaderPath + "Model/version_5/version_5.obj", false);
	fountainPool = new Model(ShaderPath + "Model/Fountain/fountain.obj", true);
	fireheap = new Model(ShaderPath + "Model/fire/fireheap.obj", false);
	waterPool = new Model(ShaderPath + "Model/pool/pool.obj",false);
	castleShader = new Shader(string(ShaderPath + "Shaders/model.vs").c_str(), 
								string(ShaderPath + "Shaders/model.fs").c_str());

	glUseProgram(0);

	batShader = new Shader(string(ShaderPath + "Shaders/AnimationModel.vs").c_str(),
							string(ShaderPath + "Shaders/AnimationModel.fs").c_str());
	batShader->use();
	batShader->setInt("texture_diffuse1",0);
	glUseProgram(0);
	//bat = new AnimationModel(batShader, "C:/Users/Yang/Desktop/OpenGL-Demo/SceneDemo/res/Model/bat/use2.FBX");
	//bat = new AnimationModel(batShader, string(ShaderPath + "Model/bat_ascii/monster/test.FBX"));

	fractTree->process(*terrain);
	//“ı”∞≤ø∑÷
	GLfloat near_plane = 1.0f, far_plane = 900.0f;
	GLfloat ws = 800.0f;
	glm::mat4 lightProjection = glm::ortho(-ws, ws, -ws, ws, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

	lightSpaceMatrix1 = lightProjection * lightView;

	glGenFramebuffers(1, &depthMapFBO);
	GenDepthTex();
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	//glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,depthMap,0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);
	glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cout << "????" << endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	shadowShader = new Shader(string(ShaderPath + "Shaders/shadow.vs").c_str(), 
								string(ShaderPath + "Shaders/shadow.fs").c_str());
	shadowShader->use();
	shadowShader->setMat4("lightSpaceMatrix", lightSpaceMatrix1);

	glm::mat4 biasMatrix{
		0.5,0.0,0.0,0.0,
		0.0,0.5,0.0,0.0,
		0.0,0.0,0.5,0.0,
		0.5,0.5,0.5,1.0
	};
	lightSpaceMatrix2 = biasMatrix*lightSpaceMatrix1;
	castleShader->use();
	castleShader->setInt("texture_diffuse1", 0);
	castleShader->setVec3("dirLight.direction", glm::normalize(lightPos));
	castleShader->setVec3("dirLight.ambient", ambientLight);
	castleShader->setVec3("dirLight.diffuse", diffuseLight);
	castleShader->setMat4("lightSpaceMatrix", lightSpaceMatrix2);

	fractTree->setLightSpace(lightSpaceMatrix1);
	snow->setLightSpace(lightSpaceMatrix1);
	fountain->setLightSpace(lightSpaceMatrix1);
	flame->setLightSpace(lightSpaceMatrix1);
	terrain->setLightSpace(lightSpaceMatrix2);
}

Scene::~Scene()
{
	delete skybox;
	//delete floor;
	delete fractTree;
	delete castle;
	delete snow;
	/*delete fireWork;*/
	//delete bat;
	delete castleShader;
	delete batShader;
	delete fountainPool;
	delete shadowShader;
	delete fireheap;
	delete flame;
	delete terrain;
}

void Scene::render(glm::mat4 model, glm::mat4 &view, glm::mat4 &projection,
	Camera::Camera &camera, float deltaTime, float timeflow)
{
	OutsideRender(model, view, projection, camera, deltaTime, timeflow);
}

void Scene::OutsideRender(glm::mat4 model, glm::mat4 &view, glm::mat4 &projection,
	Camera::Camera &camera, float deltaTime, float &timeflow)
{
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	OutsideRenderShadow(model, camera, deltaTime, timeflow);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, screenWidth, screenHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	skybox->Draw(view, projection);
	fractTree->render(model, view, projection, camera, depthMap, lightSpaceMatrix2);
	//floor->render(model, view, projection, camera, depthMap, lightSpaceMatrix2);
	fountain->Render(deltaTime, model, view, projection);
	//fireWork->Render(deltaTime, model, view, projection);
	snow->Render(deltaTime, model, view, projection);
	//debug->render(model,view,projection,depthMap);

	terrain->render(model, view, projection,depthMap);
	flame->Render(deltaTime, model, view, projection);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	castleShader->use();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-300, 52.3, -300));
	model = glm::scale(model, glm::vec3(0.007));
	castleShader->setMat4("view", view);
	castleShader->setMat4("projection", projection);
	castleShader->setMat4("model", model);
	castle->Draw(*castleShader, depthMap);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-300, 0, -170));
	model = glm::scale(model, glm::vec3(0.05));
	castleShader->setMat4("model", model);
	fountainPool->Draw(*castleShader, depthMap);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(40, 0, -160));
	castleShader->setMat4("model", model);
	fireheap->Draw(*castleShader, depthMap);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-110, 0, -190));
	model = glm::scale(model, glm::vec3(4.0f,2.0f,4.0f));
	castleShader->setMat4("model", model);
	waterPool->Draw(*castleShader, depthMap);

	glDisable(GL_CULL_FACE);
	glUseProgram(0);

	water->render(model, view, projection, deltaTime);

	//model = glm::mat4(1.0f);
	//model = glm::rotate(model, (float)glm::radians((-timeflow)*80.0f), glm::vec3(0, 1, 0));
	//model = glm::translate(model, glm::vec3(200, 130, 0));
	//model = glm::scale(model, glm::vec3(0.4));
	//bat->Draw(model, view, projection, timeflow);

}

void Scene::OutsideRenderShadow(glm::mat4 model, Camera::Camera &camera, float deltaTime, float &timeflow)
{
	shadowShader->use();
	model = glm::mat4(1.0f);
	shadowShader->setMat4("model", model);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-300, 52.3, -300));
	model = glm::scale(model, glm::vec3(0.007));
	shadowShader->setMat4("model", model);
	castle->Draw(*shadowShader, depthMap);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-300, 0, -170));
	model = glm::scale(model, glm::vec3(0.05));
	shadowShader->setMat4("model", model);
	fountainPool->Draw(*shadowShader, depthMap);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(40, 0, -160));
	shadowShader->setMat4("model", model);
	fireheap->Draw(*shadowShader, depthMap);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-110, 0, -190));
	model = glm::scale(model, glm::vec3(4.0f, 2.0f, 4.0f));
	shadowShader->setMat4("model", model);
	waterPool->Draw(*shadowShader, depthMap);

	glDisable(GL_CULL_FACE);

	model = glm::mat4(1.0f);
	shadowShader->setMat4("model", model);
	fractTree->renderShadow(shadowShader, model);

	//model = glm::mat4(1.0f);
	//model = glm::rotate(model, (float)glm::radians((-timeflow)*80.0f), glm::vec3(0, 1, 0));
	//model = glm::translate(model, glm::vec3(200, 130, 0));
	//model = glm::scale(model, glm::vec3(0.4));
	//bat->DrawShadow(model, lightSpaceMatrix1, timeflow);

	snow->RenderShadow(deltaTime);
	fountain->RenderShadow(deltaTime);
	flame->RenderShadow(deltaTime);
	terrain->renderShadow(shadowShader);
}


void Scene::GenDepthTex() {
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	GLfloat borderColor[] = { 1.0,1.0,1.0,1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindTexture(GL_TEXTURE_2D, 0);
}