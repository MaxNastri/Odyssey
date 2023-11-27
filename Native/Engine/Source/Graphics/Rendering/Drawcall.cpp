#include "Drawcall.h"
#include "Mesh.h"

namespace Odyssey
{
	void Drawcall::SetMesh(ResourceHandle<Mesh> mesh)
	{
		VertexBuffer = mesh.Get()->GetVertexBuffer();
		IndexBuffer = mesh.Get()->GetIndexBuffer();
		IndexCount = mesh.Get()->GetIndexCount();
	}
}