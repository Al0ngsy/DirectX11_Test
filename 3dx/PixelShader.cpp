#include "PixelShader.h"
#include "GraphicThrowMacros.h"
#include <d3dcompiler.h>

namespace wrl = Microsoft::WRL;

PixelShader::PixelShader(Graphic& gfx, const std::wstring& path)
{
	INFOMAN(gfx);

	wrl::ComPtr<ID3DBlob> pBlob;
	
	GFX_THROW_INFO(
		D3DReadFileToBlob(path.c_str(), &pBlob)
	);

	GFX_THROW_INFO(
		GetDevice(gfx)->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader)
	);
}

void PixelShader::Bind(Graphic& gfx) noexcept
{
	GetDeviceContext(gfx)->PSSetShader(pPixelShader.Get(), nullptr, 0u);
}
