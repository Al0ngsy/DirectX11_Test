#pragma once
#include "Graphic.h"

class Bindable
{
public:
	virtual void Bind(Graphic& gfx) noexcept = 0;
	virtual ~Bindable() = default;
protected:
	static ID3D11Device* GetDevice(Graphic& gfx) noexcept;
	static ID3D11DeviceContext* GetDeviceContext(Graphic& gfx) noexcept;
	static DxgiInfoManager& GetInfoManager(Graphic& gfx) noexcept;
};