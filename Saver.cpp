#include "stdafx.h"
#include "Saver.h"

#include <fstream>

Saver::Saver()
	: path("..\\Level.txt")
{
}

Saver::~Saver()
{
}

void Saver::Write(int level)
{
	std::ofstream out;
	out.open(path);

	if (out.is_open())
		out << "Level: " << level << " " << std::endl;
}
