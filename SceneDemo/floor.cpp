#include "floor.h"


Floor::Floor() {
	float width = 500.0f;
	float texth = 500.0f;
	float vertices[] = {
		width,0,  width,    1.0f    ,0.0,
		width,0, -width,    1.0f    ,1.0,
		-width,0, -width,    0.0f    ,1.0,

		width,0,  width,    1.0f    ,0.0,
		-width,0, -width,    0.0f    ,1.0,
		-width,0,  width,    0.0f    ,0.0
	};
	shader = new Shader(string(ShaderPath+"Shaders/floor.vs").c_str(), 
						string(ShaderPath + "Shaders/floor.fs").c_str());
	shader->use();
	for (int x = 0; x < 2; x++) {
		char buf[8];
		sprintf_s(buf, sizeof(buf), "%d", x);
		shader->setInt("floor[" + string(buf) + "]", x);
	}
	shader->setInt("shadowMap", 2);
	shader->setVec3("dirLight.direction", glm::normalize(lightPos));
	shader->setVec3("dirLight.ambient", ambientLight);
	shader->setVec3("dirLight.diffuse", diffuseLight);
	shader->setVec3("Normal", glm::vec3(0, 1, 0));
	floorTex[0].loadTexture(ShaderPath + FLOORPIC_DIR + "a0.BMP", true);
	/*floorTex[1].loadTexture(ShaderPath + FLOORPIC_DIR + "a3.BMP", true);*/
	floorTex[1].loadTexture(ShaderPath + "textures/Normal.jpg", true);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}

Floor::~Floor() {
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	delete shader;
}

void Floor::render(glm::mat4 model, glm::mat4 view, glm::mat4 projection, Camera::Camera& camera,
	GLuint &depthMap, glm::mat4 &lightSpace) {
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	shader->use();
	shader->setMat4("view", view);
	shader->setMat4("projection", projection);
	shader->setMat4("lightSpaceMatrix", lightSpace);
	shader->setInt("shadowMap", 2);
	for (int x = 0; x < 2; x++) {
		glActiveTexture(GL_TEXTURE0 + x);
		glBindTexture(GL_TEXTURE_2D, floorTex[x].textureID);
	}
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glDisable(GL_CULL_FACE);
}

void Floor::renderShadow(Shader *shadowShader, glm::mat4 model) {
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	model = glm::mat4(1.0f);
	shadowShader->setMat4("model", model);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	//glDisable(GL_CULL_FACE);
}