#pragma once
#include "Bindable.h"

class Topology : public Bindable
{
public:
	Topology(Graphic& gfx, D3D11_PRIMITIVE_TOPOLOGY type);
	void Bind(Graphic& gfx) noexcept override;
protected:
	D3D11_PRIMITIVE_TOPOLOGY type;
};