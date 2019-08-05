#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>

class TransformCBuf : public Bindable
{
public:
	TransformCBuf(Graphic& gfx, const Drawable& parent);
	void Bind(Graphic& gfx) noexcept override;
protected:
	static std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> pVcbuf;
	const Drawable& parent;
};