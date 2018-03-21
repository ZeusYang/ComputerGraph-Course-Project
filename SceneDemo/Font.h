#pragma once
#include "shader.h"
#include "texture.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#ifndef FT_FREETYPE_H
#define FT_FREETYPE_H
#endif
#include <freetype/freetype.h>
#include <map>
#include "common.h"

const float wS = 1600;
const float hS = 1200;

struct Character {
	GLuint		TextureID;
	glm::ivec2	Size;
	glm::ivec2	Bearing;
	GLuint			Advance;
};

class Font {
public:
	Font();

	~Font();

	void render(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

private:
	std::map<GLchar, Character>Characters;
	GLuint VAO, VBO;
	Shader *shader;

public:
	void GenFontTexture(FT_Library& ft, FT_Face &face);
};