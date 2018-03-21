#pragma once
#include "skybox.h"
#include "floor.h"
#include "FractalTreeSystem.h"
#include "font.h"
#include "Model.h"
#include "Snow.h"
//#include "FireWork.h"
#include "AnimationModel.h"
#include "Camera.h"
#include "Fountain.h"
#include "Flame.h"
#include "Water.h"
#include "Terrain.h"

class Scene {
public:

	Scene(float time);

	~Scene();

	void render(glm::mat4 model, glm::mat4& view, glm::mat4& projection,
		Camera::Camera &camera, float deltaTime, float timeflow);

	Terrain *terrain;

private:

	Skybox *skybox;
	Floor *floor;
	FractalSystem *fractTree;
	Model *castle;
	Model *fountainPool;
	Model *waterPool;
	Model *fireheap;
	Snow::Snow *snow;
	Water *water;
	/*FireWork::FireWork *fireWork;*/
	Fountain::Fountain *fountain;
	Flame::Flame *flame;
	AnimationModel *bat;
	/*Debug *debug;*/

	Shader *castleShader;
	Shader *batShader;
	Shader *shadowShader;

	GLuint depthMapFBO, depthMap;
	glm::mat4 lightSpaceMatrix1;
	glm::mat4 lightSpaceMatrix2;

	void OutsideRender(glm::mat4 model, glm::mat4& view, glm::mat4& projection,
		Camera::Camera &camera, float deltaTime, float &timeflow);

	void OutsideRenderShadow(glm::mat4 model, Camera::Camera &camera, float deltaTime, float &timeflow);

	void GenDepthTex();
};