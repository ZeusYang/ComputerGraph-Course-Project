#include "FractalTreeSystem.h"

FractalSystem::FractalSystem()
{
	dx = dz = 35.0f;
	dy = 30.0f;
	length = 10.0;
	radius = 0.5f;
	lengthFactor = 0.75;
	radiusFactor = 0.72;
	numTrunks = numLeafs = 0;
	numJunTrs = numJunLs = 0;
	level = 7;
}

void FractalSystem::setLightSpace(glm::mat4 &lightSpaceMatrix) {
	junShadow->use();
	junShadow->setMat4("lightSpaceMatrix", lightSpaceMatrix);
	glUseProgram(0);
}

FractalSystem::~FractalSystem()
{
	clearAll();
	delete trunkShader;
	delete leafsShader;
	delete jungleTrunkShader;
	delete jungleLeafsShader;
}

void FractalSystem::DrawInit()
{
	vector<glm::vec3>trunkVer;
	vector<glm::vec2>texcoord;
	vector<glm::vec3>normal;
	/*for(int x = 0;x < trunks.size();x ++){
	//trunkVer.push_back(trunks[x].start);
	//trunkVer.push_back(trunks[x].end);
	Geometry::Cylinder(trunks[x].start,trunks[x].end,trunks[x].radius,trunkVer,texcoord);
	//cout << trunks[x].radius << endl;
	}*/
	TrunkInit(trunks, trunkVer, texcoord, normal);

	numTrunks = trunkVer.size();
	glGenBuffers(1, &trunkVBO);
	glGenVertexArrays(1, &trunkVAO);
	glGenBuffers(1, &leafsVBO);
	glGenVertexArrays(1, &leafsVAO);
	trunkShader = new Shader(string(ShaderPath + "Shaders/trunk.vs").c_str(),
							string(ShaderPath + "Shaders/trunk.fs").c_str());
	leafsShader = new Shader(string(ShaderPath + "Shaders/leafs.vs").c_str(), 
							string(ShaderPath + "Shaders/leafs.fs").c_str());
	trunkTex.loadTexture(ShaderPath + "textures/body.bmp", true);
	leafsTex.loadTexture(ShaderPath + "textures/leafs.png", false);
	trunkShader->use();
	trunkShader->setInt("bark", 0);
	trunkShader->setInt("shadowMap", 1);
	trunkShader->setVec3("dirLight.direction", glm::normalize(lightPos));
	trunkShader->setVec3("dirLight.ambient", ambientLight);
	trunkShader->setVec3("dirLight.diffuse", diffuseLight);
	leafsShader->use();
	leafsShader->setInt("leaf", 0);
	//trunks
	int tVs = trunkVer.size() * sizeof(glm::vec3);
	int tTs = texcoord.size() * sizeof(glm::vec2);
	int tNs = normal.size() * sizeof(glm::vec3);
	glBindVertexArray(trunkVAO);
	glBindBuffer(GL_ARRAY_BUFFER, trunkVBO);
	glBufferData(GL_ARRAY_BUFFER, tVs + tTs + tNs, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, tVs, &trunkVer[0]);
	glBufferSubData(GL_ARRAY_BUFFER, tVs, tTs, &texcoord[0]);
	glBufferSubData(GL_ARRAY_BUFFER, tVs + tTs, tNs, &normal[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2),
		(GLvoid*)(trunkVer.size() * sizeof(glm::vec3)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
		(GLvoid*)(trunkVer.size() * sizeof(glm::vec3) + texcoord.size() * sizeof(glm::vec2)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	//leafs
	vector<glm::vec3>leafVer;
	vector<glm::vec2>leafTex;
	LeafInit(leafs, leafVer, leafTex);
	numLeafs = leafVer.size();
	int lVs = leafVer.size() * sizeof(glm::vec3);
	//int lTs = leafTex.size() * sizeof(glm::vec2);
	glBindVertexArray(leafsVAO);
	glBindBuffer(GL_ARRAY_BUFFER, leafsVBO);
	glBufferData(GL_ARRAY_BUFFER, lVs, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, lVs, &leafVer[0]);
	//glBufferSubData(GL_ARRAY_BUFFER, lVs, lTs, &leafTex[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (GLvoid*)(numLeafs * sizeof(glm::vec3)));
	glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);
	glBindVertexArray(0);

}

void FractalSystem::DrawJungleInit()
{
	vector<glm::vec3>trunkVer;
	vector<glm::vec2>texcoord;
	vector<glm::vec3>normal;
	TrunkInit(trunks, trunkVer, texcoord, normal);

	numJunTrs = trunkVer.size();
	glGenBuffers(1, &jungleTrunkVBO);
	glGenVertexArrays(1, &jungleTrunkVAO);
	glGenBuffers(1, &jungleLeafsVBO);
	glGenVertexArrays(1, &jungleLeafsVAO);
	jungleTrunkShader = new Shader(string(ShaderPath + "Shaders/jungleTrunk.vs").c_str(),
									string(ShaderPath + "Shaders/jungleTrunk.fs").c_str());
	jungleLeafsShader = new Shader(string(ShaderPath + "Shaders/jungleLeafs.vs").c_str(), 
									string(ShaderPath + "Shaders/jungleLeafs.fs").c_str());
	junShadow = new Shader(string(ShaderPath + "Shaders/junShadow.vs").c_str(),
							string(ShaderPath + "Shaders/junShadow.fs").c_str());
	jungleTrunkShader->use();
	jungleTrunkShader->setInt("bark", 0);
	jungleTrunkShader->setVec3("dirLight.direction", glm::normalize(lightPos));
	jungleTrunkShader->setVec3("dirLight.ambient", ambientLight);
	jungleTrunkShader->setVec3("dirLight.diffuse", diffuseLight);
	jungleLeafsShader->use();
	jungleLeafsShader->setInt("leaf", 0);

	//trunks
	int tVs = trunkVer.size() * sizeof(glm::vec3);
	int tTs = texcoord.size() * sizeof(glm::vec2);
	int iMs = instanceMatrix.size() * sizeof(glm::mat4);
	int tNs = normal.size() * sizeof(glm::vec3);
	int vs = trunkVer.size();
	int ts = texcoord.size();
	glBindVertexArray(jungleTrunkVAO);
	glBindBuffer(GL_ARRAY_BUFFER, jungleTrunkVBO);
	glBufferData(GL_ARRAY_BUFFER, tVs + tTs + iMs + tNs, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, tVs, &trunkVer[0]);
	glBufferSubData(GL_ARRAY_BUFFER, tVs, tTs, &texcoord[0]);
	glBufferSubData(GL_ARRAY_BUFFER, tVs + tTs, iMs, &instanceMatrix[0]);
	glBufferSubData(GL_ARRAY_BUFFER, tVs + tTs + iMs, tNs, &normal[0]);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2),
		(GLvoid*)(trunkVer.size() * sizeof(glm::vec3)));

	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4),
		(GLvoid*)(vs * sizeof(glm::vec3) + ts * sizeof(glm::vec2)));
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4),
		(GLvoid*)(vs * sizeof(glm::vec3) + ts * sizeof(glm::vec2) + sizeof(glm::vec4)));
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4),
		(GLvoid*)(vs * sizeof(glm::vec3) + ts * sizeof(glm::vec2) + 2 * sizeof(glm::vec4)));
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4),
		(GLvoid*)(vs * sizeof(glm::vec3) + ts * sizeof(glm::vec2) + 3 * sizeof(glm::vec4)));
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
		(GLvoid*)(vs * sizeof(glm::vec3) + ts * sizeof(glm::vec2) + iMs));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);

	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glBindVertexArray(0);

	//leafs
	vector<glm::vec3>leafVer;
	vector<glm::vec2>leafTex;
	LeafInit(leafs, leafVer, leafTex);
	numJunLs = leafVer.size();
	int lVs = leafVer.size() * sizeof(glm::vec3);
	//int lTs = leafTex.size() * sizeof(glm::vec2);
	glBindVertexArray(jungleLeafsVAO);
	glBindBuffer(GL_ARRAY_BUFFER, jungleLeafsVBO);
	glBufferData(GL_ARRAY_BUFFER, lVs + iMs, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, lVs, &leafVer[0]);
	//glBufferSubData(GL_ARRAY_BUFFER, lVs, lTs, &leafTex[0]);
	glBufferSubData(GL_ARRAY_BUFFER, lVs, iMs, &instanceMatrix[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)(0));
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (GLvoid*)(numJunLs * sizeof(glm::vec3)));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4),
		(GLvoid*)(numJunLs * sizeof(glm::vec3)));
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4),
		(GLvoid*)(numJunLs * sizeof(glm::vec3) + sizeof(glm::vec4)));
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4),
		(GLvoid*)(numJunLs * sizeof(glm::vec3) + 2 * sizeof(glm::vec4)));
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4),
		(GLvoid*)(numJunLs * sizeof(glm::vec3) + 3 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);

	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glBindVertexArray(0);

	clearAll();
}

void FractalSystem::render(glm::mat4 model, glm::mat4 view, glm::mat4 projection, Camera::Camera& camera, GLuint &depthMap, glm::mat4 &lightSpace)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//render trunks
	model = glm::translate(model, glm::vec3(-110, 0, -120));
	model = glm::scale(model, glm::vec3(3.0f));
	trunkShader->use();
	trunkShader->setMat4("model", model);
	trunkShader->setMat4("view", view);
	trunkShader->setMat4("projection", projection);
	trunkShader->setMat4("lightSpaceMatrix", lightSpace);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, trunkTex.textureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glBindVertexArray(trunkVAO);
	glDrawArrays(GL_TRIANGLES, 0, numTrunks);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	//render leafs
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);
	leafsShader->use();
	leafsShader->setMat4("model", model);
	leafsShader->setMat4("view", view);
	leafsShader->setMat4("projection", projection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, leafsTex.textureID);
	glBindVertexArray(leafsVAO);
	glDrawArrays(GL_POINTS, 0, numLeafs);
	glBindVertexArray(0);

	//render Jungles
	jungleTrunkShader->use();
	jungleTrunkShader->setMat4("view", view);
	jungleTrunkShader->setMat4("projection", projection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, trunkTex.textureID);
	glBindVertexArray(jungleTrunkVAO);
	//glDrawArrays(GL_TRIANGLES,0,numJunTrs);
	glDrawArraysInstanced(GL_TRIANGLES, 0, numJunTrs, TREE_NUMS);

	//render leafs
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);
	jungleLeafsShader->use();
	jungleLeafsShader->setMat4("view", view);
	jungleLeafsShader->setMat4("projection", projection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, leafsTex.textureID);
	glBindVertexArray(jungleLeafsVAO);
	//glDrawArrays(GL_POINTS,0,numJunLs);
	glDrawArraysInstanced(GL_POINTS, 0, numJunLs, TREE_NUMS);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_CULL_FACE);
}
void FractalSystem::renderShadow(Shader* shadowShader, glm::mat4 model)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	//render trunks
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-110, 0, -120));
	model = glm::scale(model, glm::vec3(3.0f));
	shadowShader->setMat4("model", model);
	glBindVertexArray(trunkVAO);
	glDrawArrays(GL_TRIANGLES, 0, numTrunks);
	glBindVertexArray(0);
	//render leafs
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glBindVertexArray(leafsVAO);
	glDrawArrays(GL_POINTS, 0, numLeafs);
	glBindVertexArray(0);

	//render Jungles
	junShadow->use();
	glBindVertexArray(jungleTrunkVAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, numJunTrs, TREE_NUMS);

	//render leafs
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glBindVertexArray(jungleLeafsVAO);
	glDrawArraysInstanced(GL_POINTS, 0, numJunLs, TREE_NUMS);
	glBindVertexArray(0);
	glDisable(GL_CULL_FACE);
}


void FractalSystem::clearAll()
{
	grammar.clear();
	trunks.clear();
	leafs.clear();
	cylinderTable.clear();
	instanceMatrix.clear();
}

void FractalSystem::process(Terrain &terrain)
{
	initGrammar(level);
	generateFractal();
	DrawInit();

	grammar.setResult(jungleStr);
	grammar.setLevel(TREE_LEVEL);
	generateFractal();
	JungleRange(instanceMatrix,terrain);
	DrawJungleInit();
}

void FractalSystem::initGrammar(int levels)
{
	//srand(time(NULL));
	grammar.setGrammarName("Test1");
	grammar.addGeneration('S', "F[^$X][*%X][&%X]");
	grammar.addGeneration('X', "F[^%D][&$D][/$D][*%D]");
	grammar.addGeneration('X', "F[&%D][*$D][/$D][^%D]");
	grammar.addGeneration('D', "F[^$X][*%FX][&%X]");
	grammar.setStart("S");
	grammar.iterateFor(levels, jungleStr);
}

void FractalSystem::generateFractal()
{
	trunks.clear();
	leafs.clear();
	curState.pos = glm::vec3(0, 0, 0);
	curState.dir = glm::vec3(0, 1, 0);
	curState.length = length;
	curState.level = 1;
	curState.radius = radius;

	std::stack<State>stacks;

	for (int i = 0; i < grammar.getResult().size(); i++) {
		char ch = grammar.getResult()[i];
		Trunk tmp;
		switch (ch) {
		case 'F': {
			tmp.start = curState.pos;
			curState.pos += curState.dir*(float)curState.length;
			tmp.end = curState.pos;
			//tmp.length = curState.length;
			tmp.radius = curState.radius;
			tmp.level = curState.level;
			trunks.push_back(tmp);
			break;
		}
		case '$': {
			curState.dir = Geometry::RotateY(curState.dir, dy);
			break;
		}
		case '%': {
			curState.dir = Geometry::RotateY(curState.dir, -dy);
			break;
		}
		case '^': {
			curState.dir = Geometry::RotateX(curState.dir, dx);
			break;
		}
		case '&': {
			curState.dir = Geometry::RotateX(curState.dir, -dx);
			break;
		}
		case '*': {
			curState.dir = Geometry::RotateZ(curState.dir, dz);
			break;
		}
		case '/': {
			curState.dir = Geometry::RotateZ(curState.dir, -dz);
			break;
		}
		case '[': {
			stacks.push(curState);
			curState.length *= lengthFactor;
			curState.radius *= radiusFactor;
			curState.level += 1;
			break;
		}
		case ']': {
			if (curState.level == grammar.getLevel()) {
				Trunk tm = trunks[trunks.size() - 1];
				Leaf rs;
				rs.dir = tm.end - tm.start;
				rs.pos = tm.end;
				leafs.push_back(rs);
			}
			curState = stacks.top();
			stacks.pop();
			break;
		}
		default:
			break;
		}
	}
}

void FractalSystem::LeafInit(vector<Leaf>leafs, vector<glm::vec3>&vertice, vector<glm::vec2>&texcoord)
{
	int len = leafs.size();
	vertice.reserve(len);
	//texcoord.reserve(len*6);
	//srand(time(NULL));
	for (int x = 0; x < len; x++) {
		leafs[x].dir = glm::normalize(leafs[x].dir);
		vertice.push_back(leafs[x].dir*LEAF_WIDTH + leafs[x].pos);
	}
}

void FractalSystem::TrunkInit(vector<Trunk>trunks, vector<glm::vec3>&vertice,
	vector<glm::vec2>&texcoord, vector<glm::vec3>&normal)
{
	double curRadius = radius;
	double curLength = length;
	for (int i = 0; i < level + 1; i++) {
		cylinderNode record;
		Geometry::CylinderMesh(curLength, curRadius, record.vertice, record.texcoord, record.normal);
		cylinderTable[i + 1] = record;
		curLength *= lengthFactor;
		curRadius *= radiusFactor;
	}

	int len = trunks.size();
	vertice.reserve(30 * len);
	texcoord.reserve(30 * len);
	normal.reserve(30 * len);
	for (int x = 0; x < len; x++) {
		glm::mat4 nmats(1.0f);
		glm::mat4 trans = Geometry::GetTranMat(trunks[x].start, trunks[x].end, nmats);
		int nums = cylinderTable[trunks[x].level].vertice.size();
		for (int y = 0; y < nums; y++) {
			glm::vec4 tmp = trans*glm::vec4(cylinderTable[trunks[x].level].vertice[y], 1.0f);
			vertice.push_back(glm::vec3(tmp.x, tmp.y, tmp.z));
			tmp = nmats*glm::vec4(cylinderTable[trunks[x].level].normal[y], 1.0f);
			normal.push_back(glm::vec3(tmp.x, tmp.y, tmp.z));
		}
		texcoord.insert(texcoord.end(), cylinderTable[trunks[x].level].texcoord.begin(),
			cylinderTable[trunks[x].level].texcoord.end());
	}
}

void FractalSystem::JungleRange(vector<glm::mat4>&instanceMatrix,Terrain &terrain)
{
	srand(time(NULL));
	instanceMatrix.reserve(TREE_NUMS);
	float edge = 512;
	for (int x = 0; x < TREE_NUMS; x++) {
		glm::vec3 record;
		record.y = 0.0f;
		record.x = edge*(2.0f*float(rand()) / float(RAND_MAX) - 1.0f);
		record.z = edge*(2.0f*float(rand()) / float(RAND_MAX) - 1.0f);

		if (((record.x+300)*(record.x + 300) + (record.z+300)*(record.z + 300) < 80000)) {
			x -= 1;
			continue;
		}
		else if (terrain.Travel(record.z,record.x) > 0.0) {
			x -= 1;
			continue;
		}
		glm::mat4 model(1.0f);
		model = glm::translate(model, record);
		instanceMatrix.push_back(model);
	}
}