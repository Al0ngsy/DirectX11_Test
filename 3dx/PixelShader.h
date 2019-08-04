#pragma once
#include "Bindable.h"

namespace wrl = Microsoft::WRL;

class PixelShader : public Bindable
{
public:
	PixelShader(Graphic& gfx, const std::wstring& path);
	void Bind(Graphic& gfx) noexcept override;
protected:
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
};