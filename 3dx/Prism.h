#pragma once

#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "Math.h"

namespace dx = DirectX;

class Prism
{
public:
	template<class V>
	static IndexedTriangleList<V> MakeTesselated(int baseVert)
	{
		assert(baseVert >= 3);

		const auto base = dx::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
		const auto offset = dx::XMVectorSet(0.0f, 0.0f, 2.0f, 0.0f);
		const float longitudeAngle = 2.0f * PI / baseVert;

		//

	}
};