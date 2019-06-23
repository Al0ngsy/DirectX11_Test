#include "Window.h"
#include <sstream>
#include "resource.h"

// Window Class
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
	: hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;	// using own Window procedure which overwrite some window's default proc
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = static_cast<HICON>(
		LoadImage(hInst,
				MAKEINTRESOURCE(IDI_ICON1),
				IMAGE_ICON, 32, 32, 0));
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>(
		LoadImage(hInst,
				MAKEINTRESOURCE(IDI_ICON1),
				IMAGE_ICON, 16, 16, 0));
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, GetInstance());
}

const char* Window::WindowClass::GetName() noexcept
{
	return wndClassName;	// return the static variable
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;	// return the instance of the window class
}

// Window
Window::Window(int width, int height, const char* name)
	: width(width), height(height)
{
	// calc window size client area
	RECT wr;
	wr.left = 100;
	wr.right = wr.left + width;
	wr.top = 100;
	wr.bottom = wr.top + height;
	// Error Handling
	if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU, FALSE) == 0)
	{
		throw ERR_LAST_EXCEPT();
	}
	// create window & get hWnd
	hWnd = CreateWindow(
		WindowClass::GetName(),
		name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr,
		WindowClass::GetInstance(), 
		this
	);
	// Error Handling
	if (hWnd == nullptr)
	{
		throw ERR_LAST_EXCEPT();
	}
	// newly created windows start of as hidden
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	// create graphic object
	pGfx = std::make_unique<Graphic>(hWnd);
}

Window::~Window() 
{
	DestroyWindow(hWnd);
}

void Window::SetTitle(const std::string& title)
{
	if (SetWindowText(hWnd, title.c_str()) == 0)
	{
		throw ERR_LAST_EXCEPT();
	}
}

std::optional<int> Window::ProcessMessage()
{
	MSG msg;

	//while queue has messages, remove and dispatch them
	while (PeekMessage(&msg,nullptr,0,0,PM_REMOVE))
	{
		// check for quit messages - peekmessage does not signal this via return
		if (msg.message == WM_QUIT)
		{
			return msg.wParam;
		}

		TranslateMessage(&msg);	// responsible for throwing out WM_CHAR from Key Input, not for F1, F2 ... 
		DispatchMessage(&msg);
	}

	// return empty optinal when not quitting app
	return {}; 
}

Graphic& Window::Gfx()
{
	return *pGfx;
}

LRESULT WINAPI Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// use create paratmeter passed in from createWindow() above to store window class pointer
	if ( msg == WM_NCCREATE)
	{
		// extract ptr to window class from creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		// set WinAPI-managed user data to store prt to window class
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		// forward message to window class handler
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	// use default handler if we get a message before WM_NCCREATE message
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// retrieve ptr to window class
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// forward message to window class handler
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg)
	{
	case WM_CLOSE:	// quitting the application but not destroy, that does the destructor
		PostQuitMessage( 0 );
		return 0;
	// clear keystates on window focus loss to prevent input getting "stuck"
	case WM_KILLFOCUS:
		kbrd.ClearState();
		break;
	/*****START KEYBOARD MESSAGES*****/
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN: // syskey to track ALT Key
		if ( !(lParam & 0x40000000 /*bit 30 see WM_KEYDOWN*/) || kbrd.AutoRepIsEnabled()) // filter autorepeat
		{
			kbrd.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:	// syskey to track ALT Key -> VK_MENU
		kbrd.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:
		kbrd.OnChar(static_cast<unsigned char>(wParam));
		break;
	/*****END KEYBOARD MESSAGES*****/
	/*****START MOUSE MESSAGES*****/
	case WM_MOUSEMOVE:
		POINTS pt = MAKEPOINTS(lParam);
		// inside client region
		if (pt.x >= 0 && pt.x <= width && pt.y >= 0 && pt.y <= height)
		{
			mouse.OnMouseMove(pt.x, pt.y);
			// capture mouse
			if (!mouse.IsInWindow())
			{
				SetCapture(hWnd);
				mouse.OnMouseEnter(); // set isInWindow to true
			}
		}
		// outside client region
		else 
		{
			// maintain capture if mouse was down
			if (mouse.leftIsPressed || mouse.rightIsPressed || mouse.middleIsPressed) 
			{
				mouse.OnMouseMove(pt.x, pt.y);
			}
			// button up -> release capture
			else
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}
		break;
	case WM_LBUTTONDOWN:
		pt = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(pt.x, pt.y);
		break;
	case WM_RBUTTONDOWN:
		pt = MAKEPOINTS(lParam);
		mouse.OnRightPressed(pt.x, pt.y);
		break;
	case WM_MBUTTONDOWN:
		pt = MAKEPOINTS(lParam);
		mouse.OnMiddlePressed(pt.x, pt.y);
		break;
	case WM_LBUTTONUP:
		pt = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(pt.x, pt.y);
		break;
	case WM_RBUTTONUP:
		pt = MAKEPOINTS(lParam);
		mouse.OnRightReleased(pt.x, pt.y);
		break;
	case WM_MBUTTONUP:
		pt = MAKEPOINTS(lParam);
		mouse.OnMiddleReleased(pt.x, pt.y);
		break;
	case WM_MOUSEWHEEL:
		pt = MAKEPOINTS(lParam);

		/**dirty way to handle wheel up or down**/
		/*
		// wParam contains info about up (>0) or down (<0)
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
		{
			mouse.OnWheelUp(pt.x, pt.x);
		}
		else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
		{
			mouse.OnWheelDown(pt.x, pt.y);
		}
		*/

		/**finer-resolution wheel handling**/
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(pt.x, pt.y, delta);

		break;
	/*****END MOUSE MESSAGES*****/
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

Window::Exception::Exception(int line, const char* file, HRESULT hr) noexcept
	: ErrorException(line, file), hr(hr)
{
}

const char* Window::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "Error Code: " << GetErrorCode() << std::endl
		<< "Description: " << GetErrorString() << std::endl
		<< GetOriginString();
	whatbuffer = oss.str();
	return whatbuffer.c_str();
}

const char* Window::Exception::GetType() const noexcept
{
	return "Window Error Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* MsgBuffer = nullptr;
	DWORD MsgLength = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | // aloc mem & put the error in msg buffer
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
		reinterpret_cast<LPSTR>(&MsgBuffer),	// specify a pointer to a buffer th receives the error string
		0,
		nullptr
	);
	if (MsgLength == 0)
	{
		return "Unidentfied Error Code";
	}
	std::string errorString = MsgBuffer;
	LocalFree(MsgBuffer);
	return errorString;
}

HRESULT Window::Exception::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(hr);
}
