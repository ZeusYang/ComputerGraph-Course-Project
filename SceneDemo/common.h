#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stack>

using namespace std;

#ifndef PI
#define PI 3.1415926
#endif
#ifndef PI2
#define PI2 6.2831853
#endif

#define SHADOW_WIDTH 4096
#define SHADOW_HEIGHT 4096

const GLfloat screenWidth = 1600;
const GLfloat screenHeight = 1000;
const glm::vec3 lightPos(300, 300, 0);
const glm::vec3 sunPos(900, 900, 900);
const glm::vec3 ambientLight(0.3f);
const glm::vec3 diffuseLight(0.7f);
const string ShaderPath = "../res/";