#include "App.h"

int CALLBACK WinMain(		// CallBack use stdcall instead of cdecl of std C, window need it
	HINSTANCE hIntance,		
	HINSTANCE hPrevIntance,	// useless
	LPSTR	  lpCmdLine,	// cmdline as string when called from terminal
	int       nShowCmd)		// TBD??
{
	try
	{
		App().AppLoop();
	}
	catch (const ErrorException& e)
	{
		MessageBox(
			nullptr,
			e.what(),
			e.GetType(),
			MB_OK | MB_ICONEXCLAMATION
		);
	}
	catch (const std::exception& e)
	{
		MessageBox(
			nullptr,
			e.what(),
			"Standard Exception",
			MB_OK | MB_ICONEXCLAMATION
		);
	}
	catch (...)
	{
		MessageBox(
			nullptr,
			"Do details availble",
			"Unknow Exeption",
			MB_OK | MB_ICONEXCLAMATION
		);
	}
	return -1;
}