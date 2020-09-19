#pragma once

class Saver
{
public:
	Saver();
	~Saver();

	void Write(int level);

private:
	const std::string path;
};

