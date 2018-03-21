#pragma once
#include "shader.h"
#include "texture.h"
#include <vector>
#include "common.h"

class Skybox {
public:

	Skybox();

	~Skybox();

	void Draw(glm::mat4 view, glm::mat4 projection);

private:
	unsigned int skymap;
	unsigned int skyVAO, skyVBO;
	Shader *shader;

	void loadCubeMap(vector<string> faces);

};
