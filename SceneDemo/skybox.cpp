#include "skybox.h"


	Skybox::Skybox() {
		float skyVertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f,  1.0f
		};

		//for(int x = 0;x < 108;x ++){
		//	skyVertices[x]	*= 500;
		//}

		vector<string> faces;
		faces.reserve(6);
		faces.push_back(ShaderPath + "textures/skybox7/right.jpg");
		faces.push_back(ShaderPath + "textures/skybox7/left.jpg");
		faces.push_back(ShaderPath + "textures/skybox7/top.jpg");
		faces.push_back(ShaderPath + "textures/skybox7/bottom.jpg");
		faces.push_back(ShaderPath + "textures/skybox7/back.jpg");
		faces.push_back(ShaderPath + "textures/skybox7/front.jpg");
		loadCubeMap(faces);

		glGenVertexArrays(1, &skyVAO);
		glGenBuffers(1, &skyVBO);
		glBindVertexArray(skyVAO);
		glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyVertices), &skyVertices[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glBindVertexArray(0);

		shader = new Shader(string(ShaderPath + "Shaders/skybox.vs").c_str(),
							string(ShaderPath + "Shaders/skybox.fs").c_str());
		shader->setInt("skybox", 0);
	}

	Skybox::~Skybox() {
		glDeleteVertexArrays(1, &skyVAO);
		glDeleteBuffers(1, &skyVBO);
		delete shader;
	}

	void Skybox::Draw(glm::mat4 view, glm::mat4 projection) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_FALSE);
		shader->use();
		shader->setMat4("view", glm::mat4(glm::mat3(view)));
		shader->setMat4("projection", projection);
		glBindVertexArray(skyVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skymap);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthMask(GL_TRUE);
		glDisable(GL_CULL_FACE);
	}

	void Skybox::loadCubeMap(vector<string> faces) {
		glGenTextures(1, &skymap);
		glActiveTexture(GL_TEXTURE0);
		int width, height, nrComponents;
		unsigned char* data;
		glBindTexture(GL_TEXTURE_CUBE_MAP, skymap);
		for (GLuint i = 0; i < faces.size(); i++) {
			data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
			GLenum format;
			if (nrComponents == 1)format = GL_RED;
			else if (nrComponents == 3)format = GL_RGB;
			else if (nrComponents == 4)format = GL_RGBA;
			if (data) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format,
					width, height, 0, format, GL_UNSIGNED_BYTE, data);
			}
			else {
				std::cout << "Cube texture failed to load at path: " << faces[i] << std::endl;
			}
			stbi_image_free(data);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}