#include "ResourceManager.h"
#include "VulkanContext.h"
#include "VulkanVertexBuffer.h"

namespace Odyssey
{
	void ResourceManager::Initialize(std::shared_ptr<VulkanContext> context)
	{
		m_Context = context;
	}

	ResourceHandle ResourceManager::AllocateVertexBuffer(std::vector<VulkanVertex>& vertices)
	{
		//uint32_t id = m_VertexBuffers.Add(m_Context, vertices);
		uint32_t id = m_VertexBuffers.Add(m_Context, vertices);
		return ResourceHandle{ id };
	}

	void ResourceManager::DestroyVertexBuffer(ResourceHandle handle)
	{
		m_VertexBuffers[handle.ID]->Destroy();
		m_VertexBuffers.Free(handle.ID);
	}
}