#include "Graphic.h"
#include "dxerr.h"
#include <wrl.h>
#include <sstream>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

// makro for error checking & throwing exception
#define GFX_EXCEPT_NOINFO(hr) Graphic::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphic::HrException( __LINE__,__FILE__,hr )

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphic::HrException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphic::DeviceRemovedException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw Graphic::InfoException( __LINE__,__FILE__,v);}}
#else
#define GFX_EXCEPT(hr) Graphic::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphic::DeviceRemovedException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

Graphic::Graphic(HWND hWnd)
{
	// descriptor
	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferDesc.Width = 0;
	scd.BufferDesc.Height = 0;
	// format of the pixel layout
	scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 0;
	scd.BufferDesc.RefreshRate.Denominator = 0;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	// anti aliasing
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	// this buffer should be use as render target
	// pipline -> buffer (swap swap) -> window
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	// amount of back buffer, we use 1 front and 1 back, no triple buffering here
	scd.BufferCount = 1;
	scd.OutputWindow = hWnd;
	scd.Windowed = true;
	// use standard with best(?) performance
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = 0;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// for checking results of d3d functions
	HRESULT hr;
	
	// create device and front/back buffer, swap chain and device context
	GFX_THROW_INFO(
		D3D11CreateDeviceAndSwapChain(
			nullptr,	// default adapter
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,	// not using software driver
			swapCreateFlags,// flag
			nullptr,	// feature level 9,10,11 ?
			0,
			D3D11_SDK_VERSION,
			&scd,
			&pSwapChain,	// basicaly like pSwapChain.ReleaseAndGetAddressOf(), overloaded of &
			&pDevice,		// if not want to release use GetAddressOf()
			nullptr,
			&pDeviceContext
		)
	);
	// gain acces to texture subresource in swap chain aka back buffer
	wrl::ComPtr<ID3D11Resource> pBackBuffer; // smart pointer Object to handle the release for us

	GFX_THROW_INFO(
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer)
	);
	GFX_THROW_INFO(
		pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget) // Get() return the underlying pointer
	);
}

void Graphic::EndFrame()
{
	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set();
#endif
	if (FAILED(hr = pSwapChain->Present(1u, 0)))
	{
		// remove/driver crash/overheating etc.
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else
		{
			throw GFX_EXCEPT(hr);
		}
	}
}

void Graphic::ClearBuffer(float r, float g, float b) noexcept
{
	const float color[] = { r, g, b, 1.0f };
	pDeviceContext->ClearRenderTargetView(pTarget.Get(), color);
}

void Graphic::TestDraw(float angle, float x, float y)
{
	HRESULT hr;	// for error throw back

	struct Vertex
	{
		struct
		{
			float x;
			float y;
		} pos;
		struct
		{
			float r, g, b;
		} color;
	};
	// create vertex buffer (a test color hexagon)
	// clock wise {x,y,r,g,b}
	Vertex triangle[] =
	{
		{0.0f, 0.5f, 0.0f, 0.0f, 0.0f},
		{0.35f, 0.3f, 0.0f, 0.0f, 0.0f},
		{0.35f, -0.3f, 0.0f, 0.0f, 0.0f},
		{0.0f, -0.5f, 0.0f, 0.0f, 0.0f},
		{-0.35f, -0.3f, 0.0f, 0.0f, 0.0f},
		{-0.35f, 0.3f, 0.0f, 0.0f, 0.0f},
	};

	// give it color
	triangle[0].color.g = 1.0f;
	triangle[0].color.b = 1.0f;

	triangle[1].color.g = 1.0f;
	
	triangle[2].color.r = 1.0f;
	triangle[2].color.g = 1.0f;
	
	triangle[3].color.r = 1.0f;
	
	triangle[4].color.r = 1.0f;
	triangle[4].color.b = 1.0f;
	
	triangle[5].color.b = 1.0f;

	// pDeviceContext -> if we do something with the pipeline

	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	D3D11_BUFFER_DESC bDesc = {};
	bDesc.ByteWidth = sizeof(triangle);
	bDesc.Usage = D3D11_USAGE_DEFAULT;
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA sResData = {};
	sResData.pSysMem = triangle;
	GFX_THROW_INFO(
		pDevice->CreateBuffer(&bDesc, &sResData, &pVertexBuffer)
	);
	
	// bind vertex buffer to pipeline
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	pDeviceContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

	// create index buffer
	const unsigned short indices[] =
	{
		0,1,2,
		2,3,4,
		4,5,0,
		0,2,4
	};
	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
	D3D11_BUFFER_DESC ibDesc = {};
	ibDesc.ByteWidth = sizeof(indices);
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;
	ibDesc.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isResData = {};
	isResData.pSysMem = indices;
	GFX_THROW_INFO(
		pDevice->CreateBuffer(&ibDesc, &isResData, &pIndexBuffer)
	);

	// bind index buffer to pipeline
	const UINT istride = sizeof(unsigned short);
	const UINT ioffset = 0u;
	pDeviceContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, ioffset);
	
	// create constant buffer for transformation matrix
	struct ConstantBuffer
	{
		// 4 by 4 row major matrix 
		dx::XMMATRIX transform;
	};

	const ConstantBuffer cb =
	{
		{
			// transposing make it a col major matrix like what gpu is expecting
			dx::XMMatrixTranspose(
				dx::XMMatrixRotationZ(angle) *
				dx::XMMatrixScaling(3.0f / 4.0f, 1.0f, 1.0f) *
				dx::XMMatrixTranslation(x, y, 1.0f)
			)
		}
	};
	wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.ByteWidth = sizeof(cb);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA cbResData = {};
	cbResData.pSysMem = &cb;
	GFX_THROW_INFO(
		pDevice->CreateBuffer(&cbDesc, &cbResData, &pConstantBuffer)
	);

	// bind constant buffer to vertex shader
	pDeviceContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());


	// create pixel shader
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	wrl::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(
		D3DReadFileToBlob(L"PixelShader.cso", &pBlob)	// reuse the blob pointer, with & command SmartPointer also do auto release for us before overwrite
	);
	GFX_THROW_INFO(
		pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader)
	);

	// bind pixel shader
	pDeviceContext->PSSetShader(pPixelShader.Get(), nullptr, 0);

	// create vertex shader
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	GFX_THROW_INFO(
		D3DReadFileToBlob(L"VertexShader.cso", &pBlob)
	);
	GFX_THROW_INFO(
		pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader)
	);

	// bind vertex shader
	pDeviceContext->VSSetShader(pVertexShader.Get(), nullptr, 0);

	// input layout (vertex) (2d position only)
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Color", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},

	};
	GFX_THROW_INFO(
		pDevice->CreateInputLayout(
			ied,
			(UINT)std::size(ied),
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			&pInputLayout
		)
	);

	//bind vertex layout
	pDeviceContext->IASetInputLayout(pInputLayout.Get());

	// bind render target
	pDeviceContext->OMSetRenderTargets(1u, pTarget.GetAddressOf() , nullptr);	// use pTarget.GetAddressOf() instead of &pTarget since we dont want it to auto release

	// set primitive topology to triangle list - full list of all type here: https://docs.microsoft.com/en-us/windows/win32/direct3d11/d3d10-graphics-programming-guide-primitive-topologies
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// config view port
	D3D11_VIEWPORT viewP;
	viewP.Width = 800;
	viewP.Height = 600;
	viewP.MinDepth = 0;
	viewP.MaxDepth = 1;
	viewP.TopLeftX = 0;
	viewP.TopLeftY = 0;
	pDeviceContext->RSSetViewports(1u, &viewP);

	GFX_THROW_INFO_ONLY(
		// this draw what is inside triangle[]
		//pDeviceContext->Draw( (UINT)std::size(triangle), 0u)
		// this draw based on index
		pDeviceContext->DrawIndexed( (UINT) std::size(indices), 0u, 0u)
	);
}

Graphic::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file),
	hr(hr)
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}

const char* Graphic::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
	if (!info.empty())
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
	whatbuffer = oss.str();
	return whatbuffer.c_str();
}

const char* Graphic::HrException::GetType() const noexcept
{
	return "Graphic Exception";
}

HRESULT Graphic::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphic::HrException::GetErrorString() const noexcept
{
	return DXGetErrorString(hr);
}

std::string Graphic::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription(hr, buf, sizeof(buf));
	return buf;
}

std::string Graphic::HrException::GetErrorInfo() const noexcept
{
	return info;
}


const char* Graphic::DeviceRemovedException::GetType() const noexcept
{
	return "Graphic Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

Graphic::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept
	: Exception(line, file)
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}

const char* Graphic::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatbuffer = oss.str();
	return whatbuffer.c_str();
}

const char* Graphic::InfoException::GetType() const noexcept
{
	return "Grapic Info Exception";
}

std::string Graphic::InfoException::GetErrorInfo() const noexcept
{
	return info;
}

