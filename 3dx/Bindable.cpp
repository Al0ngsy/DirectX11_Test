#include "Bindable.h"
#include <stdexcept>

ID3D11Device* Bindable::GetDevice(Graphic& gfx) noexcept
{
	return gfx.pDevice.Get();
}

ID3D11DeviceContext* Bindable::GetDeviceContext(Graphic& gfx) noexcept
{
	return gfx.pDeviceContext.Get();
}

DxgiInfoManager& Bindable::GetInfoManager(Graphic& gfx) noexcept
{
#ifndef NDEBUG
	return gfx.infoManager;
#else
	throw std::logic_error("Error, tried to access gfx.infoManager in Release config.");
#endif // NDEBUG

}
