#pragma once
#include "Bindable.h"

namespace wrl = Microsoft::WRL;

class InputLayout : public Bindable
{
public:
	InputLayout(Graphic& gfx,
				const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
				ID3DBlob* pVertexShaderByteCode);
	void Bind(Graphic& gfx) noexcept override;
protected:
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
};