#pragma once
#include "Bindable.h"
#include "GraphicThrowMacros.h"

namespace wrl = Microsoft::WRL;

class VertexBuffer : public Bindable
{
public:
	template<class V>
	VertexBuffer(Graphic& gfx, const std::vector<V>& vertices)
		:
		stride(sizeof(V))
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC bDesc = {};
		bDesc.ByteWidth = UINT (sizeof(V) * sizeof(vertices));
		bDesc.Usage = D3D11_USAGE_DEFAULT;
		bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bDesc.CPUAccessFlags = 0;
		bDesc.MiscFlags = 0;
		bDesc.StructureByteStride = sizeof(V);
		D3D11_SUBRESOURCE_DATA sResData = {};
		sResData.pSysMem = vertices.data();

		GFX_THROW_INFO(
			GetDevice(gfx)->CreateBuffer(&bDesc, &sResData, &pVertexBuffer)
		);
	}
	void Bind(Graphic& gfx) noexcept override;
protected:
	UINT stride;
	UINT offset = 0u;
	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
};