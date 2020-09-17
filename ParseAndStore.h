#pragma once
#include <fstream>
#include <map>
#include <string>

struct ParseAndStore
{
	ParseAndStore();
	~ParseAndStore();

	void Parse();

	float cannonballSpeed;
	float minTargetSize, maxTargetSize;
	float time;
	float countTarget;

private:
	void Store();
	void setValue(const std::string& key, float& value);

	std::map<std::string, float> allData;
	std::string path;
};