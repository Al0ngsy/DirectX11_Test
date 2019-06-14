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
	const float t = timer.Peek();
	std::ostringstream oss;
	oss << "Time since start: " << std::setprecision(1) << std::fixed << t << "s";
	wnd.SetTitle(oss.str());
}
