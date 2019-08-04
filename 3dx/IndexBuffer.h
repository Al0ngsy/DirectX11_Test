#pragma once
#include "Bindable.h"

namespace wrl = Microsoft::WRL;

class IndexBuffer : public Bindable
{
public:
	IndexBuffer(Graphic& gfx, const std::vector<unsigned short>& indices);
	void Bind(Graphic& gfx) noexcept override;
	UINT GetCount() const noexcept;
protected:
	UINT count;
	UINT ioffset = 0u;
	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
};