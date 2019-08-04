#include "Graphic.h"
#include "dxerr.h"
#include <wrl.h>
#include <sstream>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "GraphicThrowMacros.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;



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

	// create depth stencil state
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	wrl::ComPtr<ID3D11DepthStencilState> pDSState;
	GFX_THROW_INFO(
		pDevice->CreateDepthStencilState(&dsDesc, pDSState.ReleaseAndGetAddressOf())
	);

	// bind depth stencil state to output merger
	pDeviceContext->OMSetDepthStencilState(pDSState.Get(), 1u);

	// create depth stencil texture
	wrl::ComPtr<ID3D11Texture2D> pTxtr2d;
	D3D11_TEXTURE2D_DESC txtr2dDesc = {};
	txtr2dDesc.Width = 800u;
	txtr2dDesc.Height = 600u;
	txtr2dDesc.MipLevels = 1u;
	txtr2dDesc.ArraySize = 1u;
	txtr2dDesc.Format = DXGI_FORMAT_D32_FLOAT;
	txtr2dDesc.SampleDesc.Count = 1u;	//AA
	txtr2dDesc.SampleDesc.Quality = 0u;
	txtr2dDesc.Usage = D3D11_USAGE_DEFAULT;
	txtr2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	GFX_THROW_INFO(
		pDevice->CreateTexture2D(&txtr2dDesc, nullptr, &pTxtr2d)
	);

	// create depth stencil texture view
	D3D11_DEPTH_STENCIL_VIEW_DESC dsDescView = {};
	dsDescView.Format = DXGI_FORMAT_D32_FLOAT;
	dsDescView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsDescView.Texture2D.MipSlice = 0u;
	GFX_THROW_INFO(
		pDevice->CreateDepthStencilView(pTxtr2d.Get(), &dsDescView, &pDepthView)
	);

	// bind depth stencil texture view to output merger
	pDeviceContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDepthView.Get());

	// configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = 800.0f;
	vp.Height = 600.0f;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pDeviceContext->RSSetViewports(1u, &vp);
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
	pDeviceContext->ClearDepthStencilView(pDepthView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphic::DrawIndexed(UINT count) noexcept
{
	GFX_THROW_INFO_ONLY(
		pDeviceContext->DrawIndexed(count, 0u, 0u)
	);
}

void Graphic::SetProjection(DirectX::FXMMATRIX proj) noexcept
{
	projection = proj;
}

DirectX::XMMATRIX Graphic::GetProjection() const noexcept
{
	return projection;
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

Graphic::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept
	: Exception(line, file)
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

const char* Graphic::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatbuffer = oss.str();
	return whatbuffer.c_str();
}

const char* Graphic::InfoException::GetType() const noexcept
{
	return "Grapic Info Exception";
}

std::string Graphic::InfoException::GetErrorInfo() const noexcept
{
	return info;
}

