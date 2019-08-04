#pragma once
#include "Bindable.h"
#include "GraphicThrowMacros.h"

namespace wrl = Microsoft::WRL;

template<typename C>
class ConstantBuffer : public Bindable
{
public:
	// update function for all the constant buffer
	void Update(Graphic& gfx, const C& consts)
	{
		INFOMAN(gfx);

		D3D11_MAPPED_SUBRESOURCE msr;
		GFX_THROW_INFO(
			GetDeviceContext(gfx)->Map(
				pConstantBuffer.Get(),
				0u,
				D3D11_MAP_WRITE_DISCARD,
				0u,
				&msr
			)
		);
		memcpy(msr.pData, &consts, sizeof(consts));
		GetDeviceContext(gfx)->Unmap(
			pConstantBuffer.Get(),
			0u
		);
	}

	// constant buffer constructor with initialised Data
	ConstantBuffer(Graphic& gfx, const C& consts)
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC cbDesc = {};
		cbDesc.ByteWidth = sizeof(consts);
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		cbDesc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA cbResData = {};
		cbResData.pSysMem = &consts;

		GFX_THROW_INFO(
			GetDevice(gfx)->CreateBuffer(&cbDesc, &cbResData, &pConstantBuffer)
		);
	}

	// constant buffer constructor without init Data
	ConstantBuffer(Graphic& gfx)
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC cbDesc = {};
		cbDesc.ByteWidth = sizeof(C);
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		cbDesc.StructureByteStride = 0;

		GFX_THROW_INFO(
			GetDevice(gfx)->CreateBuffer(&cbDesc, nullptr, &pConstantBuffer)
		);
	}

protected:
	wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
};

// BINDING
template<typename C>
class VertexConstantBuffer : public ConstantBuffer<C>
{
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(Graphic& gfx) noexcept override
	{
		this->GetDeviceContext(gfx)->VSSetConstantBuffers(0u, 1u, this->pConstantBuffer.GetAddressOf());
	}
};

template<typename C>
class PixelConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::pConstantBuffer;
	using Bindable::GetDeviceContext;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(Graphic& gfx) noexcept override
	{
		GetDeviceContext(gfx)->PSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());
	}
};