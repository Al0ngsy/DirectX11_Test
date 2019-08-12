#pragma once

#include <vector>
#include <array>
#include "IndexedTriangleList.h"
#include "Math.h"

namespace dx = DirectX;

class Plane
{
public:
	template<class V>
	static IndexedTriangleList<V> MakeTesselated(int div_x, int div_y)
	{
		assert(div_x >= 1);
		assert(div_y >= 1);
		// number of vert in each dir
		const int nVert_x = div_x + 1;
		const int nVert_y = div_y + 1;
		// storage for verts (all grid point)
		std::vector<V> verts(nVert_x * nVert_y);

		constexpr float width = 2.0f;
		constexpr float height = 2.0f;
		const float side_x = width / 2.0f;
		const float side_y = height / 2.0f;
		const float divSize_x = width / (float)div_x;
		const float divSize_y = height / (float)div_y;
		
		// bottom left position
		const auto bottomleft = dx::XMVectorSet(-side_x, -side_y, 0.0f, 0.0f);

		// create and store vector for each grid point
		for (int y = 0, i = 0; y < nVert_y; y++)
		{
			const float y_len = (float)y * divSize_y;
			for (int x = 0; x < nVert_x; x++, i++)
			{
				const float x_len = (float)x * divSize_x;
				const auto vector = dx::XMVectorAdd(	// Computes the sum of two vectors
					bottomleft,
					dx::XMVectorSet(x_len, y_len, 0.0f, 0.0f)
				);
				dx::XMStoreFloat3(&verts[i].pos, vector);
			}
		}

		// create and store indices
		std::vector<unsigned int> indices;
		indices.reserve(sq(div_x * div_y) * 6);

		const auto vxy2i = [nVert_x](size_t x, size_t y)
		{
			return (unsigned short)(y * nVertices_x + x);
		};

		for (size_t y = 0; y < div_y; y++)
		{
			for (size_t x = 0; x < div_x; x++)
			{
				const std::array<unsigned short, 4> indexArray = {
					vxy2i(x, y),		//0
					vxy2i(x + 1, y),	//1
					vxy2i(x, y + 1),	//2
					vxy2i(x+1, y+1)		//3
				}
				indices.push_back(indexArray[0]);
				indices.push_back(indexArray[2]);
				indices.push_back(indexArray[1]);
				indices.push_back(indexArray[1]);
				indices.push_back(indexArray[2]);
				indices.push_back(indexArray[3]);
			}
		}

		return { std::move(verts),
				std::move(indices) };
	}

	template<class V>
	static IndexedTriangleList<V> Make()
	{
		return MakeTesselated(1, 1);
	}
};