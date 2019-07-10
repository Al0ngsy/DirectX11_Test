#pragma once
#include "HeaderOfHeader.h"
#include <d3d11.h>
#include "ErrorException.h"
#include <vector>
#include "DxgiInfoManager.h"
#include <wrl.h>

class Graphic
{
public:
	// base graphic Exception
	class Exception : public ErrorException
	{
		using ErrorException::ErrorException;
	};
	// graphic exception that return hresult
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};
	class InfoException : public Exception
	{
	public :
		InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};
public:
	Graphic(HWND hWnd);
	// rule of three - google it
	Graphic(const Graphic&) = delete;
	Graphic& operator = (const Graphic&) = delete;
	~Graphic() = default;
	// swap back and front
	void EndFrame();
	void ClearBuffer(float r, float g, float b) noexcept;

	void TestDraw();

private:
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif
	// for CreateDeviceAndSwapChain
	// declaring Smart/Com Pointer, they auto release
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
};