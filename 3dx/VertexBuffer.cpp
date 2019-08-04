#include "VertexBuffer.h"

void VertexBuffer::Bind(Graphic& gfx) noexcept
{
	GetDeviceContext(gfx)->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
}
