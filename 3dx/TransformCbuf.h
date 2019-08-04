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
	VertexConstantBuffer<DirectX::XMMATRIX> vcbuf;
	const Drawable& parent;
};