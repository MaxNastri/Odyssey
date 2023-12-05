#include "ResourceManager.h"
#include "Resource.h"
#include "VulkanContext.h"
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanTexture.h"
#include "VulkanShader.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanBuffer.h"
#include "Mesh.h"
#include "Material.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandPool.h"
#include "VulkanDescriptorLayout.h"
#include "VulkanDescriptorBuffer.h"

namespace Odyssey
{
	void ResourceManager::Initialize(std::shared_ptr<VulkanContext> context)
	{
		m_Context = context;
	}

	ResourceHandle<Material> ResourceManager::AllocateMaterial(ResourceHandle<VulkanShader> vertexShader, ResourceHandle<VulkanShader> fragmentShader)
	{
		uint32_t id = m_Materials.Add(vertexShader, fragmentShader);
		m_Materials[id]->SetID(id);
		return ResourceHandle<Material>(id, m_Materials[id].get());
	}

	ResourceHandle<VulkanBuffer> ResourceManager::AllocateBuffer(BufferType bufferType, uint32_t size)
	{
		uint32_t id = m_Buffers.Add(m_Context, bufferType, size);
		m_Buffers[id]->SetID(id);
		return ResourceHandle<VulkanBuffer>(id, m_Buffers[id].get());
	}

	ResourceHandle<VulkanVertexBuffer> ResourceManager::AllocateVertexBuffer(std::vector<VulkanVertex>& vertices)
	{
		uint32_t id = m_VertexBuffers.Add(m_Context, vertices);
		m_VertexBuffers[id]->SetID(id);
		return ResourceHandle<VulkanVertexBuffer>(id, m_VertexBuffers[id].get());
	}

	ResourceHandle<VulkanIndexBuffer> ResourceManager::AllocateIndexBuffer(std::vector<uint32_t>& indices)
	{
		uint32_t id = m_IndexBuffers.Add(m_Context, indices);
		m_IndexBuffers[id]->SetID(id);
		return ResourceHandle<VulkanIndexBuffer>(id, m_IndexBuffers[id].get());
	}

	ResourceHandle<VulkanTexture> ResourceManager::AllocateTexture(uint32_t width, uint32_t height)
	{
		uint32_t id = m_Textures.Add(m_Context, width, height);
		m_Textures[id]->SetID(id);
		return ResourceHandle<VulkanTexture>(id, m_Textures[id].get());
	}

	ResourceHandle<VulkanShader> ResourceManager::AllocateShader(ShaderType shaderType, const std::string& filename)
	{
		uint32_t id = m_Shaders.Add(m_Context, shaderType, filename);
		m_Shaders[id]->SetID(id);
		return ResourceHandle<VulkanShader>(id, m_Shaders[id].get());
	}

	ResourceHandle<VulkanGraphicsPipeline> ResourceManager::AllocateGraphicsPipeline(const VulkanPipelineInfo& info)
	{
		uint32_t id = m_GraphicsPipelines.Add(m_Context, info);
		m_GraphicsPipelines[id]->SetID(id);
		return ResourceHandle<VulkanGraphicsPipeline>(id, m_GraphicsPipelines[id].get());
	}

	ResourceHandle<VulkanCommandPool> ResourceManager::AllocateCommandPool()
	{
		uint32_t id = m_CommandPools.Add(m_Context);
		m_CommandPools[id]->SetID(id);
		return ResourceHandle<VulkanCommandPool>(id, m_CommandPools[id].get());
	}

	ResourceHandle<VulkanCommandBuffer> ResourceManager::AllocateCommandBuffer(ResourceHandle<VulkanCommandPool> commandPool)
	{
		uint32_t id = m_CommandBuffers.Add(m_Context, commandPool);
		m_CommandBuffers[id]->SetID(id);
		return ResourceHandle<VulkanCommandBuffer>(id, m_CommandBuffers[id].get());
	}

	ResourceHandle<VulkanDescriptorLayout> ResourceManager::AllocateDescriptorLayout(DescriptorType type, ShaderStage shaderStage, uint32_t bindingIndex)
	{
		uint32_t id = m_DescriptorLayouts.Add(m_Context, type, shaderStage, bindingIndex);
		m_DescriptorLayouts[id]->SetID(id);
		return ResourceHandle<VulkanDescriptorLayout>(id, m_DescriptorLayouts[id].get());
	}

	ResourceHandle<VulkanDescriptorBuffer> ResourceManager::AllocateDescriptorBuffer(ResourceHandle<VulkanDescriptorLayout> layout, uint32_t descriptorCount)
	{
		uint32_t id = m_DescriptorBuffers.Add(m_Context, layout, descriptorCount);
		m_DescriptorBuffers[id]->SetID(id);
		return ResourceHandle<VulkanDescriptorBuffer>(id, m_DescriptorBuffers[id].get());
	}

	void ResourceManager::DestroyBuffer(ResourceHandle<VulkanBuffer> handle)
	{
		m_Buffers[handle.m_ID]->Destroy();
		m_Buffers[handle.m_ID]->SetID(-1);
		m_Buffers.Remove(handle.m_ID);
	}

	void ResourceManager::DestroyVertexBuffer(ResourceHandle<VulkanVertexBuffer> handle)
	{
		m_VertexBuffers[handle.m_ID]->Destroy();
		m_VertexBuffers[handle.m_ID]->SetID(-1);
		m_VertexBuffers.Remove(handle.m_ID);
	}

	void ResourceManager::DestroyIndexBuffer(ResourceHandle<VulkanIndexBuffer> handle)
	{
		m_IndexBuffers[handle.m_ID]->Destroy();
		m_IndexBuffers[handle.m_ID]->SetID(-1);
		m_IndexBuffers.Remove(handle.m_ID);
	}

	void ResourceManager::DestroyTexture(ResourceHandle<VulkanTexture> handle)
	{
		m_Textures[handle.m_ID]->Destroy();
		m_Textures[handle.m_ID]->SetID(-1);
		m_Textures.Remove(handle.m_ID);
	}
	void ResourceManager::DestroyShader(ResourceHandle<VulkanShader> handle)
	{
		m_Shaders[handle.m_ID]->Destroy();
		m_Shaders[handle.m_ID]->SetID(-1);
		m_Shaders.Remove(handle.m_ID);
	}

	void ResourceManager::DestroyGraphicsPipeline(ResourceHandle<VulkanGraphicsPipeline> handle)
	{
		m_GraphicsPipelines[handle.m_ID]->Destroy();
		m_GraphicsPipelines[handle.m_ID]->SetID(-1);
		m_GraphicsPipelines.Remove(handle.m_ID);
	}

	void ResourceManager::DestroyCommandPool(ResourceHandle<VulkanCommandPool> handle)
	{
		m_CommandPools[handle.m_ID]->Destroy();
		m_CommandPools[handle.m_ID]->SetID(-1);
		m_CommandPools.Remove(handle.m_ID);
	}
	void ResourceManager::DestroyCommandBuffer(ResourceHandle<VulkanCommandBuffer> bufferHandle, ResourceHandle<VulkanCommandPool> poolHandle)
	{
		m_CommandBuffers[bufferHandle.m_ID]->Destroy(poolHandle);
		m_CommandBuffers[bufferHandle.m_ID]->SetID(-1);
		m_CommandBuffers.Remove(bufferHandle.m_ID);
	}
	void ResourceManager::DestroyDescriptorLayout(ResourceHandle<VulkanDescriptorLayout> handle)
	{
		m_DescriptorLayouts[handle.m_ID]->Destroy();
		m_DescriptorLayouts[handle.m_ID]->SetID(-1);
		m_DescriptorLayouts.Remove(handle.m_ID);
	}
	void ResourceManager::DestroyDescriptorBuffer(ResourceHandle<VulkanDescriptorBuffer> handle)
	{
		m_DescriptorBuffers[handle.m_ID]->Destroy();
		m_DescriptorBuffers[handle.m_ID]->SetID(-1);
		m_DescriptorBuffers.Remove(handle.m_ID);
	}
}