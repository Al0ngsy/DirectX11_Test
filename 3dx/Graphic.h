#pragma once
#include "HeaderOfHeader.h"
#include <d3d11.h>

class Graphic
{
public:
	Graphic(HWND hWnd);
	// rule of three - google it
	Graphic(const Graphic&) = delete;
	Graphic& operator = (const Graphic&) = delete;
	~Graphic();
	// swap back and front
	void EndFrame();
	void ClearBuffer(float r, float g, float b) noexcept {
		const float color[] = { r, g, b, 1.0f };
		pDeviceContext->ClearRenderTargetView(pTarget, color);
	}
private:
	// for CreateDeviceAndSwapChain
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwapChain = nullptr;
	ID3D11DeviceContext* pDeviceContext = nullptr;
	ID3D11RenderTargetView* pTarget = nullptr;
};