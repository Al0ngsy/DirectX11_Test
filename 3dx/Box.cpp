#include "Box.h"
#include "BindableBase.h"
#include "GraphicThrowMacros.h"

Box::Box(Graphic& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist)
	:
	r(rdist(rng)),
	droll(ddist(rng)),
	dpitch(ddist(rng)),
	dyaw(ddist(rng)),
	dphi(odist(rng)),
	dtheta(odist(rng)),
	dchi(odist(rng)),
	chi(adist(rng)),
	theta(adist(rng)),
	phi(adist(rng))
{	
	// going with static through DrawableBase so that all the cubes share the same stuff below with exception of transform
	if (!IsStaticInited())
	{
		struct Vertex
		{
			struct
			{
				float x;
				float y;
				float z;
			} pos;
		};
		const std::vector<Vertex> vertices =
		{
			{ -1.0f,-1.0f,-1.0f },
			{ 1.0f,-1.0f,-1.0f },
			{ -1.0f,1.0f,-1.0f },
			{ 1.0f,1.0f,-1.0f },
			{ -1.0f,-1.0f,1.0f },
			{ 1.0f,-1.0f,1.0f },
			{ -1.0f,1.0f,1.0f },
			{ 1.0f,1.0f,1.0f },
		};

		// Vertex Buffer
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));

		auto pvs = std::make_unique<VertexShader>(gfx, L"VertexShader.cso");

		auto pvsbc = pvs->GetBytecode();

		// Vertex Shader
		AddStaticBind(std::move(pvs));

		// Pixel Shader
		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PixelShader.cso"));

		const std::vector<unsigned short> indices =
		{
			0,2,1, 2,3,1,
			1,3,5, 3,7,5,
			2,6,3, 3,6,7,
			4,5,7, 4,7,6,
			0,4,2, 2,4,6,
			0,1,4, 1,5,4
		};

		// Index Buffer
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));

		// Face color
		struct ConstantBuffer2
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			} face_colors[6];
		};
		const ConstantBuffer2 cb2 =
		{
			{
				{ 1.0f,0.0f,1.0f },
				{ 1.0f,0.0f,0.0f },
				{ 0.0f,1.0f,0.0f },
				{ 0.0f,0.0f,1.0f },
				{ 1.0f,1.0f,0.0f },
				{ 0.0f,1.0f,1.0f },
			}
		};

		// Pixel Constant Buffer
		AddStaticBind(std::make_unique<PixelConstantBuffer<ConstantBuffer2>>(gfx, cb2));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};

		// Input Layout
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

		// Topology
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		// for 2nd, 3rd ... boxes
		SetIndexFromStatic();
	}

	// Transform
	// Is for each cube diferent, so we add it through drawable
	AddBind(std::make_unique<TransformCBuf>(gfx, *this));
}

void Box::Update(float dt) noexcept
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
		DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}
