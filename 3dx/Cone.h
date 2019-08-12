#pragma once
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "Math.h"

namespace dx = DirectX;

class Cone
{
public:
	template<class V>
	static IndexedTriangleList<V> MakeTesselated(int baseVert)
	{
		assert(baseVert >= 3);

		const auto base = dx::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
		const float longitudeAxis = 2.0f * PI / baseVert;

		// base vert
		std::vector<V> vertices;
		for (int i = 0; i < basever; i++)
		{
			vertices.emplace_back();
			auto v = dx::XMVector3Transform(
				base,
				dx::XMMatrixRotationZ(longitudeAxis, *i)
			);
			dx::XMStoreFloat3(&vertices.back().pos, v);
		}

		// center
		vertices.emplace_back();
		vertices.back().pos = { 0.0f, 0.0f, -1.0f };
		const auto iCenter = (unsigned short)(vertices.size() - 1); // pos of center in vertices

		// tip
		vertices.emplace_back();
		vertices.back().pos = { 0.0f, 0.0f, 1.0f };
		const auto iTip = (unsigned short)(vertices.size() - 1); // pos of tip in vertices

		// base indices
		std::vector<unsigned short> indices;
		for (unsigned short i = 0; i < baseVert; i++)
		{
			// clockwise starting at center
			indices.push_back(iCenter);
			indices.push_back((i + 1) % baseVert);
			indices.push_back(i);
		}

		// cone indices
		for (unsigned short i = 0; i < baseVert; i++)
		{
			// clockwie starting at tip
			indices.push_back(iTip);
			indices.push_back((i + 1) % baseVert);
			indices.push_back(i);
		}

		// indexed triangle list expect (vertices_in, indexes_in)
		return {std::move(vertices),
				std::move(indices)}
	}

	template<class V>
	static IndexedTriangleList<V> Make()
	{
		// dafault make return cone with base = 24 corner
		return MakeTesselated<V>(24);
	}

};