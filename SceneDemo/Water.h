#pragma once
#include "shader.h"
#include "texture.h"
#include "common.h"
#include <vector>

class Water
{
public:
	Water(float times,int w = 100, int h = 100);
	~Water();

	void InitVertices();
	void WaterSin(float &ime);
	void DataTransfer();
	void render(glm::mat4 model, glm::mat4 view, glm::mat4 projection, float time);

private:
	GLuint waterVAO, waterVBO,waterEBO;
	CTexture surface;
	int width, height;
	Shader *waterShader;
	vector<glm::vec3> vertices;
	vector<int>indices;
	float gap, timeCounter;
	float time;
};

