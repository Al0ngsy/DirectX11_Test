#pragma once
#include "HeaderOfHeader.h"

class Window
{
private:
	// singleton manages registration/cleanup of window class
	class WindowClass
	{
	public:
		static const char*	GetName() noexcept;
		static HINSTANCE	GetInstance() noexcept;
	private:
		WindowClass() noexcept;	// Constructor
		~WindowClass();			// Destructor
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator = (const WindowClass&) = delete;
		
		static constexpr const char* wndClassName = "Just a little Program";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	Window(int width, int height, const char* name) noexcept;
	~Window();
	Window(const Window&) = delete;
	Window& operator = (const Window&) = delete;

private:
	static LRESULT WINAPI HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
private:
	int width = 0;
	int height = 0;
	HWND hWnd;
};