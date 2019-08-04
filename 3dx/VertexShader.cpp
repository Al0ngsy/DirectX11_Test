#include "VertexShader.h"
#include "GraphicThrowMacros.h"
#include <d3dcompiler.h>

VertexShader::VertexShader(Graphic& gfx, const std::wstring& path)
{
	INFOMAN(gfx);

	GFX_THROW_INFO(
		D3DReadFileToBlob(path.c_str(), &pBlob)
	);

	GFX_THROW_INFO(
		GetDevice(gfx)->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader)
	);
}

void VertexShader::Bind(Graphic& gfx) noexcept
{
	GetDeviceContext(gfx)->VSSetShader(pVertexShader.Get(), nullptr, 0u);
}

ID3DBlob* VertexShader::GetBytecode() const noexcept
{
	return pBlob.Get();
}
