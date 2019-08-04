#include "TransformCbuf.h"

TransformCBuf::TransformCBuf(Graphic& gfx, const Drawable& parent)
	:
	vcbuf(gfx),
	parent(parent)
{
}

void TransformCBuf::Bind(Graphic& gfx) noexcept
{
	vcbuf.Update(gfx,
		DirectX::XMMatrixTranspose(
			parent.GetTransformXM() * gfx.GetProjection()
		)
	);
	vcbuf.Bind(gfx);
}
