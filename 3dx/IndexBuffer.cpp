#include "IndexBuffer.h"
#include "GraphicThrowMacros.h"

IndexBuffer::IndexBuffer(Graphic& gfx, const std::vector<unsigned short>& indices)
	:
	count( (UINT)indices.size() )
{
	INFOMAN(gfx);
	D3D11_BUFFER_DESC ibDesc = {};
	ibDesc.ByteWidth = UINT( count * sizeof(unsigned short));
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;
	ibDesc.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isResData = {};
	isResData.pSysMem = indices.data();
	
	GFX_THROW_INFO(
		GetDevice(gfx)->CreateBuffer(&ibDesc, &isResData, &pIndexBuffer)
	);
}

void IndexBuffer::Bind(Graphic& gfx) noexcept
{
	GetDeviceContext(gfx)->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, ioffset);
}

UINT IndexBuffer::GetCount() const noexcept
{
	return count;
}
