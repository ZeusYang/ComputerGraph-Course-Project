#include "AnimationModel.h"

AnimationModel::AnimationModel(Shader* shader, string path)
{
	m_NumBones = 0;
	m_pShader = shader;
	m_pCkAiScene = new ck_aiScene();
	texture.loadTexture(string(ShaderPath+"textures/bat.png"));
	loadModel(path);
	//std::cout << path << std::endl;
	m_pShader->use();
	m_pShader->setInt("tex", 1);
	m_pShader->setVec3("dirLight.direction", glm::normalize(lightPos));
	m_pShader->setVec3("dirLight.ambient", ambientLight);
	m_pShader->setVec3("dirLight.diffuse", diffuseLight);
	m_pShadow = new Shader(string(ShaderPath + "Shaders/AniShadow.vs").c_str(),
							string(ShaderPath + "Shaders/AniShadow.fs").c_str());
}

void AnimationModel::PreDraw(Shader* shader, float timeInSeconds, vector<glm::mat4>&transforms, bool constrain)
{
	if (m_pCkAiScene->mAnimationVec.size() && constrain)
	{
		transforms.clear();
		glm::mat4 identify(1.0f);
		float tickPerSecond = m_pCkAiScene->mAnimationVec[0]->mTicksPerSecond != 0 ?
			m_pCkAiScene->mAnimationVec[0]->mTicksPerSecond : 25.0f;
		float timeInTicks = timeInSeconds*tickPerSecond;
		float animationTime = fmod(timeInTicks, m_pCkAiScene->mAnimationVec[0]->mDuration);
		//获取当前动画在第一个动作里面的比例
		readNodeHierachy(animationTime, m_pCkAiScene->mRootNode, identify);
		transforms.resize(m_NumBones);
	}
	for (GLuint i = 0; i < m_NumBones; i++)
	{
		transforms[i] = m_BoneInfo[i].FinalTransformation;
	}
	shader->use();
	for (GLuint i = 0; i < transforms.size(); i++)
	{
		char numStr[6];
		sprintf_s(numStr,sizeof(numStr), "%d", i);
		string strNum(numStr);
		shader->setMat4("gBones[" + strNum + "]", transforms[i]);
	}
}

void AnimationModel::DrawShadow(glm::mat4 model, glm::mat4 lightSpaceMatrix, float timeInSeconds)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	PreDraw(m_pShadow, timeInSeconds, transforms, true);
	m_pShadow->setMat4("model", model);
	m_pShadow->setMat4("lightSpaceMatrix", lightSpaceMatrix);
	for (int i = 0; i < meshes.size(); i++) {
		meshes[i].Draw(m_pShadow);
	}
	glUseProgram(0);
	glDisable(GL_CULL_FACE);
}

void AnimationModel::Draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection, float timeInSeconds)
{
	PreDraw(m_pShader, timeInSeconds, transforms, false);
	m_pShader->setMat4("model", model);
	m_pShader->setMat4("view", view);
	m_pShader->setMat4("projection", projection);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture.textureID);
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw(m_pShader);
	}
	glDisable(GL_CULL_FACE);
}

void AnimationModel::readNodeHierachy(float animationTime, const ck_aiNode* pNode, const glm::mat4& parentTransform)
{
	string nodeName(pNode->mNodeName);
	const ck_aiAnimation* pAnimation = m_pCkAiScene->mAnimationVec[0];
	glm::mat4 nodeTransform(1.0f);
	ChangeaiMatrix4x4ToMat4(nodeTransform, pNode->mTransform);
	const ck_aiAnimNode* pNodeAnim = FindNodeAnim(pAnimation, nodeName);
	if (pNodeAnim)
	{
		glm::vec3 scaling;
		CalcInterpolatedScaling(scaling, animationTime, pNodeAnim);
		glm::mat4 scalingM(1.0f);
		scalingM = glm::scale(scalingM, scaling);
		aiQuaternion rotationQ;
		CalcInterpolatedRotation(rotationQ, animationTime, pNodeAnim);
		glm::mat4 rotationM(1.0f);
		ChangeaiMatrix3x3ToMat4(rotationM, rotationQ.GetMatrix());
		glm::vec3 translation;
		CalcInterpolatedPosition(translation, animationTime, pNodeAnim);
		glm::mat4 transformM(1.0f);
		transformM = glm::translate(transformM, translation);
		nodeTransform = transformM*rotationM*scalingM;
	}
	glm::mat4 GlobalTransform = parentTransform*nodeTransform;
	if (m_BoneMapping.find(nodeName) != m_BoneMapping.end())
	{
		GLuint boneIndex = m_BoneMapping[nodeName];
		m_BoneInfo[boneIndex].FinalTransformation =
			m_GlobalInverseTransform*GlobalTransform*m_BoneInfo[boneIndex].BoneOffset;
	}
	for (GLuint i = 0; i < pNode->mChildrenVec.size(); i++)
	{
		readNodeHierachy(animationTime, pNode->mChildrenVec[i], GlobalTransform);
	}

}
//插值比例缩放矩阵
void AnimationModel::CalcInterpolatedScaling(glm::vec3& out, float animationTime, const ck_aiAnimNode* pNodeAnim)
{
	if (pNodeAnim->mScalingKeys.size() == 1)
	{
		out.x = pNodeAnim->mScalingKeys[0].mValue.x;
		out.y = pNodeAnim->mScalingKeys[0].mValue.y;
		out.z = pNodeAnim->mScalingKeys[0].mValue.z;
		return;
	}
	GLuint scaleIndex = FindScaleKey(animationTime, pNodeAnim);
	GLuint nextScaleIndex = scaleIndex + 1;
	assert(nextScaleIndex < pNodeAnim->mScalingKeys.size());
	float deltaTime = pNodeAnim->mScalingKeys[nextScaleIndex].mTime - pNodeAnim->mScalingKeys[scaleIndex].mTime;
	float factor = (animationTime - (float)pNodeAnim->mScalingKeys[scaleIndex].mTime) / deltaTime;
	aiVector3D startScale = pNodeAnim->mScalingKeys[nextScaleIndex].mValue;
	aiVector3D endScale = pNodeAnim->mScalingKeys[nextScaleIndex].mValue;
	out.x = startScale.x*(1.0f - factor) + endScale.x*factor;
	out.y = startScale.y*(1.0f - factor) + endScale.y*factor;
	out.z = startScale.z*(1.0f - factor) + endScale.z*factor;
}

//根据时间插值当前的旋转四元数
void AnimationModel::CalcInterpolatedRotation(aiQuaternion& out, float animationTime, const ck_aiAnimNode* pNodeAnim)
{
	if (pNodeAnim->mRotationKeys.size() == 1)
	{
		out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}
	GLuint rotationIndex = FindRotationKey(animationTime, pNodeAnim);
	GLuint nextRotationIndex = (rotationIndex + 1);
	assert(nextRotationIndex < pNodeAnim->mRotationKeys.size());
	float deltaTime = pNodeAnim->mRotationKeys[nextRotationIndex].mTime -
		pNodeAnim->mRotationKeys[rotationIndex].mTime;
	float factor = (animationTime - (float)pNodeAnim->mRotationKeys[rotationIndex].mTime) / deltaTime;
	assert(factor >= 0.0f&&factor <= 1.0f);
	const aiQuaternion& startRotationQ = pNodeAnim->mRotationKeys[rotationIndex].mValue;
	const aiQuaternion& endRotationQ = pNodeAnim->mRotationKeys[nextRotationIndex].mValue;
	aiQuaternion::Interpolate(out, startRotationQ, endRotationQ, factor);
	out = out.Normalize();
}

void AnimationModel::CalcInterpolatedPosition(glm::vec3& out, float animationTime, const ck_aiAnimNode* pNodeAnim)
{
	if (pNodeAnim->mPositionKeys.size() == 1)
	{
		out.x = pNodeAnim->mPositionKeys[0].mValue.x;
		out.y = pNodeAnim->mPositionKeys[0].mValue.y;
		out.z = pNodeAnim->mPositionKeys[0].mValue.z;
		return;
	}
	GLuint positionIndex = FindPositionKey(animationTime, pNodeAnim);
	GLuint nextPositionIndex = positionIndex + 1;
	assert(nextPositionIndex < pNodeAnim->mPositionKeys.size());
	float deltaTime = pNodeAnim->mPositionKeys[nextPositionIndex].mTime -
		pNodeAnim->mPositionKeys[positionIndex].mTime;
	float factor = (animationTime - (float)pNodeAnim->mPositionKeys[positionIndex].mTime) / deltaTime;
	assert(factor >= 0.0f && factor <= 1.0f);
	aiVector3D startPosition = pNodeAnim->mPositionKeys[positionIndex].mValue;
	aiVector3D endPosition = pNodeAnim->mPositionKeys[nextPositionIndex].mValue;
	out.x = startPosition.x*(1.0f - factor) + endPosition.x*factor;
	out.y = startPosition.y*(1.0f - factor) + endPosition.y*factor;
	out.z = startPosition.z*(1.0f - factor) + endPosition.z*factor;
}

GLuint AnimationModel::FindRotationKey(float animationTime, const ck_aiAnimNode* pNodeAnim)
{
	for (GLuint i = 0; i < pNodeAnim->mRotationKeys.size() - 1; i++)
	{
		if (animationTime >= (float)pNodeAnim->mRotationKeys[i].mTime &&
			animationTime <= (float)pNodeAnim->mRotationKeys[i + 1].mTime)
		{//找到了当前动作介于这两帧之间
			return i;
		}
	}
	assert(0);
}

GLuint AnimationModel::FindScaleKey(float animationTime, const ck_aiAnimNode* pNodeAnim)
{
	for (GLuint i = 0; i < pNodeAnim->mScalingKeys.size() - 1; i++)
	{
		if (animationTime >= (float)pNodeAnim->mScalingKeys[i].mTime &&
			animationTime <= (float)pNodeAnim->mScalingKeys[i + 1].mTime)
		{
			return i;
		}
	}
	assert(0);
}

GLuint AnimationModel::FindPositionKey(float animationTime, const ck_aiAnimNode* pNodeAnim)
{
	for (GLuint i = 0; i < pNodeAnim->mPositionKeys.size() - 1; i++)
	{
		if (animationTime >= pNodeAnim->mPositionKeys[i].mTime&&
			animationTime <= pNodeAnim->mPositionKeys[i + 1].mTime)
		{
			return i;
		}
	}
	assert(0);
}

ck_aiAnimNode* AnimationModel::FindNodeAnim(const ck_aiAnimation* pAnimation, string nodeName)
{
	for (int i = 0; i < pAnimation->mChannelVec.size(); i++)
	{
		if (pAnimation->mChannelVec[i]->mNodeAnimName == nodeName)
		{
			return pAnimation->mChannelVec[i];
		}
	}
	return nullptr;
}

AnimationModel::~AnimationModel()
{
	for (int i = 0; i < m_pCkAiScene->mAnimationVec.size(); i++)
	{
		for (int j = 0; j < m_pCkAiScene->mAnimationVec[i]->mChannelVec.size(); j++)
		{
			delete m_pCkAiScene->mAnimationVec[i]->mChannelVec[j];
			m_pCkAiScene->mAnimationVec[i]->mChannelVec[j] = NULL;
		}
		delete m_pCkAiScene->mAnimationVec[i];
		m_pCkAiScene->mAnimationVec[i] = NULL;
	}
	delete m_pCkAiScene;
	m_pCkAiScene = NULL;
}

void AnimationModel::loadModel(string path)
{
	Assimp::Importer import;
	//ifstream test(path);
	//if (!test) {
	//	std::cout << "Yes!" << std::endl;
	//}
	if (_access(path.c_str(), 0) != -1) {
		std::cout << "file exists!" << std::endl;
	}
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	//if (!scene) {
	//	std::cout << "Could not load the file at path: " << path << std::endl;
	//	return;
	//}
	for (int i = 0; i < scene->mNumAnimations; i++)
	{
		std::cout << "!!!" << std::endl;
		ck_aiAnimation* ckanim = new ck_aiAnimation();
		ckanim->mDuration = scene->mAnimations[i]->mDuration;
		ckanim->mTicksPerSecond = scene->mAnimations[i]->mTicksPerSecond;
		for (int j = 0; j < scene->mAnimations[i]->mNumChannels; j++)
		{
			aiNodeAnim* anim = scene->mAnimations[i]->mChannels[j];
			ck_aiAnimNode* ckanimNode = new ck_aiAnimNode();
			for (int k = 0; k < anim->mNumPositionKeys; k++)
			{
				ck_aiVectorKey positionkey;
				positionkey.mTime = anim->mPositionKeys[k].mTime;
				positionkey.mValue = anim->mPositionKeys[k].mValue;
				ckanimNode->mPositionKeys.push_back(positionkey);
			}
			for (int k = 0; k < anim->mNumRotationKeys; k++)
			{
				ck_aiQuaternionKey quatkey;
				quatkey.mTime = anim->mRotationKeys[k].mTime;
				quatkey.mValue = anim->mRotationKeys[k].mValue;
				ckanimNode->mRotationKeys.push_back(quatkey);
			}
			for (int k = 0; k < anim->mNumScalingKeys; k++)
			{
				ck_aiVectorKey scalekey;
				scalekey.mTime = anim->mScalingKeys[k].mTime;
				scalekey.mValue = anim->mScalingKeys[k].mValue;
				ckanimNode->mScalingKeys.push_back(scalekey);
			}
			ckanimNode->mNodeAnimName = anim->mNodeName.C_Str();
			ckanim->mChannelVec.push_back(ckanimNode);
		}
		m_pCkAiScene->mAnimationVec.push_back(ckanim);
	}
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}

	m_GlobalInverseTransform = glm::mat4(1.0f);
	ChangeaiMatrix4x4ToMat4(m_GlobalInverseTransform, scene->mRootNode->mTransformation);
	m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform);//对其进行一个转置()

	this->directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene, NULL);
}

void AnimationModel::processNode(aiNode* node, const aiScene* scene, ck_aiNode* parent)
{
	ck_aiNode* curNode = new ck_aiNode();
	if (parent == NULL)
	{//证明当前节点是根节点
		curNode->mNodeName = node->mName.data;
		curNode->mTransform = node->mTransformation;
		m_pCkAiScene->mRootNode = curNode;
	}
	else {
		//当前节点不是根节点
		curNode->mNodeName = node->mName.data;
		curNode->mTransform = node->mTransformation;
		parent->mChildrenVec.push_back(curNode);
	}
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	for (int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, curNode);
	}
}
//将aiMesh转成我们自己的Mesh
AnimationMesh::AnimationMesh AnimationModel::processMesh(aiMesh * mesh, const aiScene * scene)
{

	vector<AnimationMesh::Vertex> vertices;//定义该Mesh所关联的顶点信息
	vector<GLuint> indices;//定义该Mesh所关联的索引信息
	vector<AnimationMesh::Texture> textures;//定义该Mesh所关联的纹理信息
	vector<AnimationMesh::VertexBoneData> bones;//该Mesh所关联的骨骼信息
	bones.resize(mesh->mNumVertices);
	//process vertex
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		AnimationMesh::Vertex vertex;
		vertex.Position.x = mesh->mVertices[i].x;
		vertex.Position.y = mesh->mVertices[i].y;
		vertex.Position.z = mesh->mVertices[i].z;
		vertex.Normal.x = mesh->mNormals[i].x;
		vertex.Normal.y = mesh->mNormals[i].y;
		vertex.Normal.z = mesh->mNormals[i].z;
		if (mesh->mTextureCoords[0])
		{
			vertex.Texcoord.x = mesh->mTextureCoords[0][i].x;
			vertex.Texcoord.y = mesh->mTextureCoords[0][i].y;
		}
		else vertex.Texcoord = glm::vec2(0.0f, 0.0f);
		vertices.push_back(vertex);
	}
	//process indices;
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		for (GLuint j = 0; j < mesh->mFaces[i].mNumIndices; j++)
		{
			indices.push_back(mesh->mFaces[i].mIndices[j]);
		}
	}
	//process material
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		vector<AnimationMesh::Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		vector<AnimationMesh::Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}
	//process bones
	for (GLuint i = 0; i < mesh->mNumBones; i++)
	{
		unsigned int boneIndex = 0;
		string boneName(mesh->mBones[i]->mName.data);//获取该骨骼的名称
		if (m_BoneMapping.find(boneName) == m_BoneMapping.end()) {
			boneIndex = m_NumBones;
			m_NumBones++;
			AnimationMesh::BoneInfo bi;
			m_BoneInfo.push_back(bi);
		}
		else {
			boneIndex = m_BoneMapping[boneName];
		}
		m_BoneMapping[boneName] = boneIndex;
		ChangeaiMatrix4x4ToMat4(m_BoneInfo[boneIndex].BoneOffset, mesh->mBones[i]->mOffsetMatrix);
		for (GLuint j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
			//mNumWeights表示该骨骼点所影响的顶点
			//这里我们是将每个骨骼对象保存在具体的Mesh中，所以该骨骼所关联的顶点ID就是保存在Mesh中的顶点ID
			unsigned int vertexID = mesh->mBones[i]->mWeights[j].mVertexId;
			float weight = mesh->mBones[i]->mWeights[j].mWeight;
			bones[vertexID].AddBoneData(boneIndex, weight);
		}
	}

	return AnimationMesh::AnimationMesh(vertices, indices, textures, bones);
}

vector<AnimationMesh::Texture> AnimationModel::loadMaterialTextures(aiMaterial * mat, aiTextureType type, string typeName)
{
	vector<AnimationMesh::Texture> textures;
	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		string path = string(directory + "/" + str.C_Str()).c_str();
		if (texturesLoaded.find(path) == texturesLoaded.end())
		{
			//path = "test2/male.dds";
			AnimationMesh::Texture texture;
			/*for(int x = 0;x < path.size();x ++){
			if(path[x] == '\\')path[x] = '/';
			}*/
			//texture.id = CreateTextureFromFile(path.c_str());
			texture.id = CTexture::loadDDS(path.c_str());
			texture.type = typeName;
			texture.path = path;
			textures.push_back(texture);
			texturesLoaded[path] = texture;
		}
		else {
			textures.push_back(texturesLoaded[path]);
		}
	}
	return textures;
}

void AnimationModel::ChangeaiMatrix4x4ToMat4(glm::mat4& out, const aiMatrix4x4& mat)
{
	assert(sizeof(out) == sizeof(mat));
	memcpy(&out, &mat, sizeof(mat));
	out = glm::transpose(out);
}

void AnimationModel::ChangeaiMatrix3x3ToMat4(glm::mat4& out, const aiMatrix3x3& mat)
{
	out[0][0] = mat.a1; out[0][1] = mat.a2; out[0][2] = mat.a3; out[0][3] = 0;
	out[1][0] = mat.b1; out[1][1] = mat.b2; out[1][2] = mat.b3; out[1][3] = 0;
	out[2][0] = mat.c1; out[2][1] = mat.c2; out[2][2] = mat.c3; out[2][3] = 0;
	out[3][0] = 0;      out[3][1] = 0;      out[3][2] = 0;      out[3][3] = 1.0f;
	out = glm::transpose(out);
}