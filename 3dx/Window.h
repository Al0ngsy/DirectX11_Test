#pragma once
#include "HeaderOfHeader.h"
#include "ErrorException.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphic.h"
#include <optional>
#include <memory>

class Window
{
public:
	class Exception : public ErrorException
	{
		using ErrorException::ErrorException;
	public:
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
	};
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hr;
	};
	class NoGfxException : public Exception
	{
	public:
		using Exception::Exception;
		const char* GetType() const noexcept override;
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
	// for testing purpose
	void SetTitle(const std::string& title);
	static std::optional<int> ProcessMessage();
	Graphic& Gfx();
public:
	Keyboard kbrd;
	Mouse mouse;
private:
	static LRESULT WINAPI HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
private:
	int width = 0;
	int height = 0;
	HWND hWnd;
	std::unique_ptr<Graphic> pGfx;
	// optinal or unique_ptr see: https://stackoverflow.com/questions/44856701/what-to-use-stdoptional-or-stdunique-ptr?rq=1
};

// Macro For Error Exeption
#define ERR_EXCEPT(hr) Window::HrException(__LINE__, __FILE__, hr);
#define ERR_LAST_EXCEPT() Window::HrException(__LINE__, __FILE__, GetLastError());
#define ERR_NOGFX_EXCEPT() Window::NoGfxException(__LINE__, __FILE__);