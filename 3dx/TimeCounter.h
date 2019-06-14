#pragma once
#include <chrono>

class TimeCounter
{
public:
	TimeCounter();
	float Mark();
	float Peek() const;
private:
	std::chrono::steady_clock::time_point last;
};