#include "Terrain.h"



Terrain::Terrain(string path)
{
	numVertex = 0;
	scale = 150.0f;
	LoadHeighmap(path);
	GenerateTerrain();
	terrainShader = new Shader(string(ShaderPath + "Shaders/terrain.vs").c_str(),
		string(ShaderPath + "Shaders/terrain.fs").c_str());
	terrainTex[0].loadTexture(ShaderPath + "textures/floor/a0.BMP", true);
	//terrainTex[1].loadTexture(ShaderPath + "textures/floor/HighTile.bmp", true);
	terrainTex[2].loadTexture(ShaderPath + "textures/floor/lowestTile.bmp", true);
	Normal.loadTexture(ShaderPath + "textures/Normal.jpg",true);
	terrainShader->setInt("terrain1", 0);
	//terrainShader->setInt("terrain2", 1);
	terrainShader->setInt("terrain3", 2);
	terrainShader->setInt("normal", 3);
	terrainShader->setVec3("dirLight.direction", glm::normalize(lightPos));
	terrainShader->setVec3("dirLight.ambient", ambientLight);
	terrainShader->setVec3("dirLight.diffuse", diffuseLight);
}

Terrain::~Terrain()
{
	glDeleteBuffers(1, &terrainVBO);
	glDeleteBuffers(1, &terrainEBO);
	glDeleteVertexArrays(1, &terrainVAO);
	if (!terrainShader) {
		delete terrainShader;
	}
	stbi_image_free(HeightData);
}

void Terrain::LoadHeighmap(string &path)
{
	HeightData = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
	if (!HeightData) {
		std::cout << "Failed to load the Height map at path:" << path << std::endl;
		return;
	}
}

float Terrain::GetTrueHeight(int i, int j)
{
	unsigned char tmp = HeightData[GetIndex(i,j)];
	int ret = (int)tmp;
	return (float)ret / 255.0f;

}

void Terrain::setLightSpace(glm::mat4 &lightSpace)
{
	terrainShader->use();
	terrainShader->setMat4("lightSpaceMatrix", lightSpace);
	glUseProgram(0);
}

float Terrain::GetHeight(float i, float j)
{
	return GetTrueHeight(i, j)*scale;
}

void Terrain::render(glm::mat4 model, glm::mat4 view, glm::mat4 projection, GLuint &depthMap)
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	terrainShader->use();
	terrainShader->setInt("terrain1", 0);
	//terrainShader->setInt("terrain2", 1);
	terrainShader->setInt("terrain3", 2);
	terrainShader->setInt("normal", 3);
	terrainShader->setMat4("view", view);
	terrainShader->setMat4("projection", projection);
	terrainShader -> setInt("shadowMap", 4);
	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, terrainTex[0].textureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, terrainTex[1].textureID);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, terrainTex[2].textureID);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, Normal.textureID);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glBindVertexArray(terrainVAO);
	glDrawElements(GL_TRIANGLES, numVertex, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glDisable(GL_CULL_FACE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Terrain::renderShadow(Shader* shadowShader)
{
	shadowShader->use();
	glm::mat4 model = glm::mat4(1.0f);
	shadowShader->setMat4("model", model);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glBindVertexArray(terrainVAO);
	glDrawElements(GL_TRIANGLES, numVertex, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glDisable(GL_CULL_FACE);
}

int Terrain::GetIndex(int i, int j)
{
	return (i*width + j)*nrComponents;
}

void Terrain::GenerateTerrain()
{
	vector<glm::vec3>vertices;
	vector<int> indices;
	float w2 = width / 2.0f;
	float z2 = height / 2.0f;
	vertices.reserve(width*height);
	indices.reserve(width*height * 4);
	for (int x = 0; x < height; x++) {
		for (int y = 0; y < width; y++) {
			glm::vec3 record;
			record.x = y - w2;
			record.z = x - z2;
			record.y = GetHeight(x, y);
			vertices.push_back(record);
		}
	}
	
	for (int x = 0; x < height - 1; x++) {
		for (int y = 0; y < width - 1; y++) {
			indices.push_back(x*width+y);
			indices.push_back((x + 1)*width + y);
			indices.push_back((x + 1)*width + y + 1);

			indices.push_back(x*width + y);
			indices.push_back((x + 1)*width + y + 1);
			indices.push_back(x*width + y + 1);
		}
	}
	numVertex = indices.size();
	glGenBuffers(1, &terrainVBO);
	glGenBuffers(1, &terrainEBO);
	glGenVertexArrays(1, &terrainVAO);
	glBindVertexArray(terrainVAO);

	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0],GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

float Terrain::Travel(float i, float j)
{
	int is = i + 512;
	int js = j + 512;
	if (is >= 1020)is = 1020;
	else if (is <= 10)is = 10;
	if (js >= 1020)js = 1020;
	else if (js <= 10)js = 10;
	float ret = GetHeight(is, js);

	//cout << is << js << "->" << ret << endl;
	return ret;
}