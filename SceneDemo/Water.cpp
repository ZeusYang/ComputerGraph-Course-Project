#include "Water.h"



Water::Water(float times,int w,int h)
	:width(w),height(h)
{
	gap = 1.0f;
	timeCounter = 0.0;
	time = times;
	InitVertices();
	WaterSin(time);
	DataTransfer();
	surface.loadTexture(string(ShaderPath + "Textures/Water02.jpg"),true);
	waterShader = new Shader(string(ShaderPath + "Shaders/water.vs").c_str(),
							string(ShaderPath + "Shaders/water.fs").c_str());
}


Water::~Water()
{
	if (!waterShader) {
		delete waterShader;
	}
	vertices.clear();
	indices.clear();
}

void Water::InitVertices()
{
	vertices.reserve(width*height);
	float w2 = -width / 2.0f;
	float h2 = -height / 2.0f;
	for (int x = 0; x < height; x++) {
		for (int y = 0; y < width; y++) {
			glm::vec3 record;
			record.x = (y + w2)*gap;
			record.y = 10;
			record.z = (x + h2)*gap;
			//cout << record.x << "," << record.z << " -- ";
			vertices.push_back(record);
		}
		//cout << endl;
	}
	indices.reserve(width*height*3);
	for (int x = 0; x < height - 1; x++) {
		for (int y = 0; y < width - 1; y++) {
			indices.push_back(x*width + y);
			indices.push_back((x + 1)*width + y);
			indices.push_back((x + 1)*width + y + 1);

			indices.push_back(x*width + y);
			indices.push_back((x + 1)*width + y + 1);
			indices.push_back(x*width + y + 1);
		}
	}

	glGenVertexArrays(1, &waterVAO);
	glGenBuffers(1, &waterVBO);
	glGenBuffers(1, &waterEBO);
}

void Water::DataTransfer()
{
	glBindVertexArray(waterVAO);
	glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*vertices.size(), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3)*vertices.size(), &vertices[0]);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*indices.size(), &indices[0], GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
	//std::cout << indices.size() << std::endl;
}

void Water::WaterSin(float &times)
{
	int len = vertices.size();
	float xd = width / 2.0f * gap;
	float zd = height / 2.0f * gap;
	for (int x = 0; x < len; x++) {
		float dist = sqrt(pow(vertices[x].x-xd,2)+pow(vertices[x].z+zd,2));
		const int waveNumber = 20;
		float wave[waveNumber];
		float sum = 0;
		for (int j = 0; j < waveNumber; j++) {
			wave[j] = 0.05*sin(dist * 2 / waveNumber*3.14 + times*0.3*3.14);
			sum += wave[j];
		}
		vertices[x].y = sum + 8.0f;
	}
}
void Water::render(glm::mat4 model, glm::mat4 view, glm::mat4 projection,float deltatime)
{
	timeCounter += deltatime;
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	if (timeCounter > 0.10) {
		time++;
		WaterSin(time);
		timeCounter = 0.0f;
	}
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-110, 0, -320));
	//WaterSin(time);
	DataTransfer();
	waterShader->use();
	waterShader->setInt("water", 0);
	waterShader->setMat4("view", view);
	waterShader->setMat4("model", model);
	waterShader->setMat4("projection", projection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, surface.textureID);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDisable(GL_BLEND);
}
