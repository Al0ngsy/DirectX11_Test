#pragma once
#include <vector>
#include <DirectXMath.h>

namespace dx = DirectX;

template<class T>
class IndexedTriangleList
{
public:
	IndexedTriangleList() = default;
	IndexedTriangleList(std::vector<T> vertices_in, std::vector<unsigned short> indices_in)
		:
		vertices(std::move(vertices_in)),
		indices(std::move(indices_in))
	{
		// at least 1 triangle
		assert(vertices.size() > 2);
		assert(indices.size() % 3 == 0);
	}
	void Transform(dx::FXMMATRIX matrix)
	{
		for (auto& v : vertices)
		{
			const dx::XMMATRIX pos = dx::XMLoadFloat3(&v.pos);
			dx::XMStoreFloat3(&v.pos, 
				dx::XMVector3Transform(
					pos, 
					matrix
				)
			);
		}
	}

public:
	std::vector<T> vertices;
	std::vector<unsigned short> indices;
};