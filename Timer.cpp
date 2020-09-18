#include "stdafx.h"
#include "Timer.h"

Timer::Timer()
	: endTime(0)
	, timeNow(0)
{
}

Timer::~Timer()
{
}

void Timer::Start()
{
	start = steady_clock::now();
	timeNow = 0;
}

void Timer::SetEndTime(float end)
{
	endTime = end;
}

int Timer::TimeNow()
{
	auto now = steady_clock::now();
	auto dur = now - start;

	timeNow = duration_cast<seconds>(dur).count();
	
	return timeNow;
}

float Timer::GetEndTime() const
{
	return endTime;
}

bool Timer::IsTimeEnd()
{
	if (timeNow >= endTime)
		return true;

	return false;
}
