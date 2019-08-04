#pragma once
#include "Bindable.h"

namespace wrl = Microsoft::WRL;

class VertexShader : public Bindable
{
public:
	VertexShader(Graphic& gfx, const std::wstring& path);
	void Bind(Graphic& gfx) noexcept override;
	ID3DBlob* GetBytecode() const noexcept;
protected: 
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	wrl::ComPtr<ID3DBlob> pBlob;
};