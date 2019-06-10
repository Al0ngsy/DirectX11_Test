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
	UnregisterClass(GetName(), GetInstance());
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
{
	// calc window size client area
	RECT wr;
	wr.left = 100;
	wr.right = wr.left + width;
	wr.top = 100;
	wr.bottom = wr.top + height;
	// Error Handling
	if (FAILED(
		AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU, FALSE)
	))
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
	// show Window
	ShowWindow(hWnd, SW_SHOW);
}

Window::~Window() 
{
	DestroyWindow(hWnd);
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
	/*****END KYBOARD MESSAGES*****/
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
