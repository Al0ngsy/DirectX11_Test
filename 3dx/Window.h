#pragma once
#include "HeaderOfHeader.h"
#include "ErrorException.h"

class Window
{
public:
	class Exception : public ErrorException
	{
	public:
		Exception(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept override;
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
	private:
		// HRESULT is the window error code
		HRESULT hr;
	};
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
	Window(int width, int height, const char* name);
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

// Macro For Error Exeption
#define ERR_EXCEPT(hr) Window::Exception(__LINE__, __FILE__, hr);
#define ERR_LAST_EXCEPT() Window::Exception(__LINE__, __FILE__, GetLastError());