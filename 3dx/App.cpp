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

	wnd.Gfx().TestDraw(
		-timer.Peek(),
		0.0f,
		0.0f
	);

	wnd.Gfx().TestDraw(
		timer.Peek(),
		// normalize mouse -> 0 ~ 2 then translate to zero centric
		// in pixel coord y go down, in normalize device coord y go up therefore need inverse
		//(wnd.mouse.GetPosX() / 400.0f) - 1.0f,
		//-(wnd.mouse.GetPosY() / 300.0f) + 1.0f
		0.0f,
		0.0f
		);
	wnd.Gfx().EndFrame();
}
