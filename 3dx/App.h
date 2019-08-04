#pragma once
#include "Window.h"
#include "TimeCounter.h"

class App
{
public:
	App();
	int Go();
	~App();
private:
	void CreateFrame();
private:
	Window wnd;
	TimeCounter timer;
	std::vector<std::unique_ptr<class Box>> boxes;
};