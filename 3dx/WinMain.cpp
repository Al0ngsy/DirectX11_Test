#include "Window.h"

int CALLBACK WinMain(		// CallBack use stdcall instead of cdecl of std C, window need it
	HINSTANCE hIntance,		
	HINSTANCE hPrevIntance,	// useless
	LPSTR	  lpCmdLine,	// cmdline as string when called from terminal
	int       nShowCmd)		// TBD??
{
	Window wnd(640, 480, "A Little Window");
	Window wnd2nd(300, 300, "Another Little Window");

	// message
	MSG msg;
	BOOL gResult;
	while ( gResult = (GetMessage(	
						&msg,
						nullptr,// get message from all window
						0,		// get all
						0		// get all
	)) > 0)	
	{
		TranslateMessage(&msg);	// responsible for throwing out WM_CHAR from Key Input, not for F1, F2 ... 
		DispatchMessage(&msg);
	}

	if (gResult == -1)
		return -1;

	return msg.wParam;

}