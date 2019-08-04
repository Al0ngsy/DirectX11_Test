#include "Topology.h"

Topology::Topology(Graphic& gfx, D3D11_PRIMITIVE_TOPOLOGY type)
	:
	type(type)
{
}

void Topology::Bind(Graphic& gfx) noexcept
{
	GetDeviceContext(gfx)->IASetPrimitiveTopology(type);
}