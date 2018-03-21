#include "Grammar.h"

void Grammar::clear()
{
	generations.clear();
}

int Grammar::find(char ch)
{
	for (int i = 0; i < generations.size(); i++) {
		if (generations[i].first == ch)
			return i;
	}
	return -1;
}

string Grammar::search(char ch)
{
	int id = find(ch);
	if (id == -1) {
		string ret;
		ret.push_back(ch);
		return ret;
	}
	int num = generations[id].second.size();

	int index = rand() % num;

	return generations[id].second[index];
}

void Grammar::addGeneration(const char& ch, const string& ref)
{
	int id = find(ch);
	if (id == -1) {
		pair<char, vector<string> > temp;
		temp.first = ch;
		temp.second.push_back(ref);
		generations.push_back(temp);
		return;
	}
	generations[id].second.push_back(ref);
}

void Grammar::iterateFor(int num, string &record)
{
	setLevel(num);
	srand(time(NULL));
	result = start;
	for (int i = 0; i < num; i++) {
		string tmp = "";
		for (int j = 0; j < result.size(); j++) {
			string t = search(result[j]);
			tmp += t;
		}
		result = tmp;
		if (i == 3)record = tmp;
	}
}

void Grammar::setGrammarName(const string& ref) {
	Gname = ref;
}

void Grammar::setStart(const string& ref)
{
	start = ref;
}

void Grammar::setResult(string &target)
{
	result = target;
}

void Grammar::setLevel(int num)
{
	level = num;
}

string Grammar::getGrammarName()
{
	return Gname;
}

int Grammar::getLevel()
{
	return level;
}

string Grammar::getResult()
{
	return result;
}