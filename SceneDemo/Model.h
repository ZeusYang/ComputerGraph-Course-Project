#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);
GLuint loadDDS(const char * filename);

class Model
{
public:
	/*  Model Data */
	vector<Texture> textures_loaded;
	vector<Mesh> meshes;
	string directory;
	bool gammaCorrection;
	bool loadWay;

	/*  Functions   */
	// constructor, expects a filepath to a 3D model.
	Model(string const &path, bool way = true, bool gamma = false);

	// draws the model, and thus all its meshes
	void Draw(Shader shader, GLuint shadowMap);

private:
	void loadModel(string const &path);

	void processNode(aiNode *node, const aiScene *scene);

	Mesh processMesh(aiMesh *mesh, const aiScene *scene);

	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};
