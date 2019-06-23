#include "App.h"
#include <sstream>
#include <iomanip>

App::App()
	: wnd(800,600,"My Little Window.")
{}

int App::AppLoop()
{
	while ( true )
	{
		// process all messages pending, but not block new messages
		if (const auto ecode = Window::ProcessMessage())
		{
			// if return optinal has value
			return *ecode;
		}
		CreateFrame();
	}
}

void App::CreateFrame()
{
	const float t = sin(timer.Peek()) / 2.0f + 0.5f;
	wnd.Gfx().ClearBuffer(t,t,t);
	wnd.Gfx().EndFrame();
}
