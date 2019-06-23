#include "Graphic.h"
#pragma comment(lib, "d3d11.lib")

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

	
	// create device and front/back buffer, swap chain and device context
	D3D11CreateDeviceAndSwapChain(
		nullptr,	// default adapter
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,	// not using software driver
		0,			// flag
		nullptr,	// feature level 9,10,11 ?
		0,
		D3D11_SDK_VERSION,
		&scd,
		&pSwapChain,	// pointer to pointer swap chain
		&pDevice,		// pointer to device pointer
		nullptr,
		&pDeviceContext // pointer to device context pointer
		);

	// gain acces to texture subresource in swap chain aka back buffer
	ID3D11Resource* pBackBuffer = nullptr;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));
	pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pTarget);
	pBackBuffer->Release();
}

Graphic::~Graphic()
{
	// reference counting, therefore release if exist
	if (pDevice != nullptr)
	{
		pDevice->Release();
	}

	if (pSwapChain != nullptr)
	{
		pSwapChain->Release();
	}

	if (pDeviceContext != nullptr)
	{
		pDeviceContext->Release();
	}

	if (pTarget != nullptr)
	{
		pTarget->Release();
	}
}

void Graphic::EndFrame()
{
	pSwapChain->Present(1u, 0);
}
