#include "stdafx.h"

#include "ParseAndStore.h"
#include <iostream>

ParseAndStore::ParseAndStore()
	: cannonballSpeed(0.0f)
	, minTargetSize(0.0f)
	, maxTargetSize(0.0f)
	, time(0.0f)
	, countTarget(0.0f)
	, path("..//GameSettings.txt")
{
}

ParseAndStore::~ParseAndStore()
{
}

void ParseAndStore::Parse()
{
	std::ifstream in(path);

	if (in.is_open())
	{
		std::string nameSetting;
		float num;

		while (in >> nameSetting >> num)
			allData[nameSetting] = num;
	}

	in.close();

	Store();
}

void ParseAndStore::Store()
{

		for (const auto& data : allData)
		{
			const std::string& nameSetting = data.first;

			if (nameSetting == "CannonballSpeed:")
				setValue(nameSetting, cannonballSpeed);
			else if (nameSetting == "countTarget:")
				setValue(nameSetting, countTarget);
			else if (nameSetting == "minTargetSize:")
				setValue(nameSetting, minTargetSize);
			else if (nameSetting == "maxTargetSize:")
				setValue(nameSetting, maxTargetSize);
			else if (nameSetting == "Time:")
				setValue(nameSetting, time);
		}
}

void ParseAndStore::setValue(const std::string & key, float& value)
{
	value = allData[key];
}
