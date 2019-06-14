#include "TimeCounter.h"

using namespace std::chrono;

TimeCounter::TimeCounter()
{
	last = steady_clock::now();
}

float TimeCounter::Mark()
{
	const auto old = last;
	last = steady_clock::now();
	const duration<float> frameTime = last - old;
	return frameTime.count();
}

float TimeCounter::Peek() const
{
	return duration<float>(steady_clock::now() - last).count();
}

