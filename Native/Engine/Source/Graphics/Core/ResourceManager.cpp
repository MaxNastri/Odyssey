#include "ResourceManager.h"
#include "VulkanContext.h"
#include "VulkanVertexBuffer.h"

namespace Odyssey
{
	void ResourceManager::Initialize(std::shared_ptr<VulkanContext> context)
	{
		m_Context = context;
	}

	ResourceHandle<VulkanVertexBuffer> ResourceManager::AllocateVertexBuffer(std::vector<VulkanVertex>& vertices)
	{
		//uint32_t id = m_VertexBuffers.Add(m_Context, vertices);
		uint32_t id = m_VertexBuffers.Add(m_Context, vertices);
		return ResourceHandle<VulkanVertexBuffer>{ id, m_VertexBuffers[id].get() };
	}

	VulkanVertexBuffer* ResourceManager::GetVertexBuffer(ResourceHandle<VulkanVertexBuffer> handle)
	{
		return m_VertexBuffers[handle.m_ID].get();
	}

	void ResourceManager::DestroyVertexBuffer(ResourceHandle<VulkanVertexBuffer> handle)
	{
		m_VertexBuffers[handle.m_ID]->Destroy();
		m_VertexBuffers.Remove(handle.m_ID);
	}
}