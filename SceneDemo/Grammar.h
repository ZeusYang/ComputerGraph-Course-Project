#pragma once
#include "shader.h"
#include "texture.h"
#include <vector>
#include <map>
#include <ctime>
using namespace std;

class Grammar {
private:
	string Gname;
	int level;
	string start;
	string result;
	vector<pair<char, vector<string> > >generations;

public:

	Grammar() {}

	void clear();

	int find(char ch);
	string search(char ch);

	void addGeneration(const char& ch, const string& ref);
	void iterateFor(int num, string &record);

	void setGrammarName(const string& ref);
	void setStart(const string& ref);
	void setLevel(int num);
	void setResult(string& target);

	string getGrammarName();
	int getLevel();
	string getResult();
};