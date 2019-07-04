#include "Graphic.h"
#include "dxerr.h"
#include <wrl.h>
#include <sstream>

#pragma comment(lib, "d3d11.lib")

namespace wrl = Microsoft::WRL;

// makro for error checking & throwing exception
#define GFX_EXCEPT_NOINFO(hr) Graphic::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphic::HrException( __LINE__,__FILE__,hr )

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphic::HrException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphic::DeviceRemovedException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#else
#define GFX_EXCEPT(hr) Graphic::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphic::DeviceRemovedException( __LINE__,__FILE__,(hr) )
#endif

Graphic::Graphic(HWND hWnd)
{
	// descriptor
	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferDesc.Width = 0;
	scd.BufferDesc.Height = 0;
	// format of the pixel layout
	scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 0;
	scd.BufferDesc.RefreshRate.Denominator = 0;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	// anti aliasing
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	// this buffer should be use as render target
	// pipline -> buffer (swap swap) -> window
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	// amount of back buffer, we use 1 front and 1 back, no triple buffering here
	scd.BufferCount = 1;
	scd.OutputWindow = hWnd;
	scd.Windowed = true;
	// use standard with best(?) performance
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = 0;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// for checking results of d3d functions
	HRESULT hr;
	
	// create device and front/back buffer, swap chain and device context
	GFX_THROW_INFO(
		D3D11CreateDeviceAndSwapChain(
			nullptr,	// default adapter
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,	// not using software driver
			swapCreateFlags,// flag
			nullptr,	// feature level 9,10,11 ?
			0,
			D3D11_SDK_VERSION,
			&scd,
			&pSwapChain,	// basicaly like pSwapChain.ReleaseAndGetAddressOf(), overloaded of &
			&pDevice,		// if not want to release use GetAddressOf()
			nullptr,
			&pDeviceContext
		)
	);
	// gain acces to texture subresource in swap chain aka back buffer
	wrl::ComPtr<ID3D11Resource> pBackBuffer; // smart pointer Object to handle the release for us

	GFX_THROW_INFO(
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer)
	);
	GFX_THROW_INFO(
		pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget) // Get() return the underlying pointer
	);
}

void Graphic::EndFrame()
{
	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set();
#endif
	if (FAILED(hr = pSwapChain->Present(1u, 0)))
	{
		// remove/driver crash/overheating etc.
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else
		{
			throw GFX_EXCEPT(hr);
		}
	}
}

void Graphic::ClearBuffer(float r, float g, float b) noexcept
{
	const float color[] = { r, g, b, 1.0f };
	pDeviceContext->ClearRenderTargetView(pTarget.Get(), color);
}

Graphic::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file),
	hr(hr)
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}

const char* Graphic::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
	if (!info.empty())
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
	whatbuffer = oss.str();
	return whatbuffer.c_str();
}

const char* Graphic::HrException::GetType() const noexcept
{
	return "Graphic Exception";
}

HRESULT Graphic::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphic::HrException::GetErrorString() const noexcept
{
	return DXGetErrorString(hr);
}

std::string Graphic::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription(hr, buf, sizeof(buf));
	return buf;
}

std::string Graphic::HrException::GetErrorInfo() const noexcept
{
	return info;
}


const char* Graphic::DeviceRemovedException::GetType() const noexcept
{
	return "Graphic Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}
