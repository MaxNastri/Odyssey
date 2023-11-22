#include "ResourceManager.h"
#include "VulkanContext.h"
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanTexture.h"
#include "VulkanShader.h"

namespace Odyssey
{
	void ResourceManager::Initialize(std::shared_ptr<VulkanContext> context)
	{
		m_Context = context;
	}

	ResourceHandle<VulkanVertexBuffer> ResourceManager::AllocateVertexBuffer(std::vector<VulkanVertex>& vertices)
	{
		uint32_t id = m_VertexBuffers.Add(m_Context, vertices);
		return ResourceHandle<VulkanVertexBuffer>(id, m_VertexBuffers[id].get());
	}

	ResourceHandle<VulkanIndexBuffer> ResourceManager::AllocateIndexBuffer(std::vector<uint32_t>& indices)
	{
		uint32_t id = m_IndexBuffers.Add(m_Context, indices);
		return ResourceHandle<VulkanIndexBuffer>(id, m_IndexBuffers[id].get());
	}

	ResourceHandle<VulkanTexture> ResourceManager::AllocateTexture(uint32_t width, uint32_t height)
	{
		uint32_t id = m_Textures.Add(m_Context, width, height);
		return ResourceHandle<VulkanTexture>(id, m_Textures[id].get());
	}

	ResourceHandle<VulkanShader> ResourceManager::AllocateShader(ShaderType shaderType, const std::string& filename)
	{
		uint32_t id = m_Shaders.Add(m_Context, shaderType, filename);
		return ResourceHandle<VulkanShader>(id, m_Shaders[id].get());
	}

	void ResourceManager::DestroyVertexBuffer(ResourceHandle<VulkanVertexBuffer> handle)
	{
		m_VertexBuffers[handle.m_ID]->Destroy();
		m_VertexBuffers.Remove(handle.m_ID);
	}

	void ResourceManager::DestroyIndexBuffer(ResourceHandle<VulkanIndexBuffer> handle)
	{
		m_IndexBuffers[handle.m_ID]->Destroy();
		m_IndexBuffers.Remove(handle.m_ID);
	}

	void ResourceManager::DestroyTexture(ResourceHandle<VulkanTexture> handle)
	{
		m_Textures[handle.m_ID]->Destroy();
		m_Textures.Remove(handle.m_ID);
	}
	void ResourceManager::DestroyShader(ResourceHandle<VulkanShader> handle)
	{
		m_Shaders[handle.m_ID]->Destroy();
		m_Shaders.Remove(handle.m_ID);
	}
}