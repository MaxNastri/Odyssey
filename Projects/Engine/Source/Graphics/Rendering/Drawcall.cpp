#include "Drawcall.h"

namespace Odyssey
{
	Drawcall::Drawcall(ResourceID vertexBufferID, ResourceID indexBufferID, uint32_t indexCount)
	{
		VertexBufferID = vertexBufferID;
		IndexBufferID = indexBufferID;
		indexCount = indexCount;
	}
}