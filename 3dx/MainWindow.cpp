#include <Windows.h>
#include <string>
#include <sstream>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:	// quitting the application
		PostQuitMessage(69);
		break;
	case WM_KEYDOWN: // handle key input
		if (wParam == 'F')
		{
			SetWindowText(hWnd, "Respects");
		}
		break;
	case WM_KEYUP:	// handle key input
		if (wParam == 'F')
		{
			SetWindowText(hWnd, "And again just another Window");
		}
		break;
	case WM_CHAR:	// for case sensitive text input
		{
			static std::string title;
			title.push_back((char)wParam);
			SetWindowText(hWnd, title.c_str());
		}
		break;
	case WM_LBUTTONDOWN:	// handle left mouse button input
		{
			const POINTS pt = MAKEPOINTS(lParam);
			std::ostringstream oss;
			oss << "(" << pt.x << ";" << pt.y << ")";
			SetWindowText(hWnd, oss.str().c_str());
		}
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
};

int CALLBACK WinMain(		// CallBack use stdcall instead of cdecl of std C, window need it
	HINSTANCE hIntance,		
	HINSTANCE hPrevIntance,	// useless
	LPSTR	  lpCmdLine,	// cmdline as string when called from terminal
	int       nShowCmd)		// TBD??
{

	const auto pClassName = "3dxClass";
	// register window class
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;	// using own Window procedure which overwrite some window's default proc
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hIntance;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = pClassName;
	wc.hIconSm = nullptr;

	RegisterClassEx( &wc );

	// create window intance
	HWND hWnd = CreateWindowEx(
		0, 
		pClassName,
		"Just another Window",
		WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU,
		300,
		300,
		640,
		480,
		nullptr,
		nullptr,
		hIntance,
		nullptr
	);

	// show the created window intance
	ShowWindow( hWnd, SW_SHOW );

	// message
	MSG msg;
	BOOL gResult;
	while ( gResult = (GetMessage(	&msg,
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
	else
		return msg.wParam;

}