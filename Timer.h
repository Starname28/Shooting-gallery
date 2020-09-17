#pragma once
#include <chrono>

using namespace std::chrono;

class Timer
{
public:
	Timer();
	~Timer();

	void Start();
	void SetEndTime(float end);

	int TimeNow();
	float GetEndTime() const;
	bool IsTimeEnd();
	
private:

	float endTime;
	int timeNow;

	steady_clock::time_point start;

};

