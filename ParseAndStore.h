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
	float level;

private:
	void Store();
	void setValue(const std::string& key, float& value);
	void Reading(std::ifstream& in);

	std::map<std::string, float> allData;
	const std::string pathFirst;
	const std::string pathSecond;
};