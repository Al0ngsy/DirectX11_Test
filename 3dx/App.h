#pragma once
#include "Window.h"
#include "TimeCounter.h"

class App
{
public:
	App();
	int AppLoop();
private:
	void CreateFrame();
private:
	Window wnd;
	TimeCounter timer;
};