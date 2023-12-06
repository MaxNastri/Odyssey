#include "ResourceManager.h"
#include "Resource.h"
#include "VulkanContext.h"
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanTexture.h"
#include "VulkanShaderModule.h"
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
		s_Context = context;
	}

	ResourceHandle<VulkanBuffer> ResourceManager::AllocateBuffer(BufferType bufferType, uint32_t size)
	{
		uint32_t id = s_Buffers.Add(s_Context, bufferType, size);
		s_Buffers[id]->SetID(id);
		return ResourceHandle<VulkanBuffer>(id, s_Buffers[id].get());
	}

	ResourceHandle<VulkanVertexBuffer> ResourceManager::AllocateVertexBuffer(std::vector<VulkanVertex>& vertices)
	{
		uint32_t id = s_VertexBuffers.Add(s_Context, vertices);
		s_VertexBuffers[id]->SetID(id);
		return ResourceHandle<VulkanVertexBuffer>(id, s_VertexBuffers[id].get());
	}

	ResourceHandle<VulkanIndexBuffer> ResourceManager::AllocateIndexBuffer(std::vector<uint32_t>& indices)
	{
		uint32_t id = s_IndexBuffers.Add(s_Context, indices);
		s_IndexBuffers[id]->SetID(id);
		return ResourceHandle<VulkanIndexBuffer>(id, s_IndexBuffers[id].get());
	}

	ResourceHandle<VulkanTexture> ResourceManager::AllocateTexture(uint32_t width, uint32_t height)
	{
		uint32_t id = s_Textures.Add(s_Context, width, height);
		s_Textures[id]->SetID(id);
		return ResourceHandle<VulkanTexture>(id, s_Textures[id].get());
	}

	ResourceHandle<VulkanShaderModule> ResourceManager::AllocateShaderModule(ShaderType shaderType, const std::string& filename)
	{
		uint32_t id = s_Shaders.Add(s_Context, shaderType, filename);
		s_Shaders[id]->SetID(id);
		return ResourceHandle<VulkanShaderModule>(id, s_Shaders[id].get());
	}

	ResourceHandle<VulkanGraphicsPipeline> ResourceManager::AllocateGraphicsPipeline(const VulkanPipelineInfo& info)
	{
		uint32_t id = s_GraphicsPipelines.Add(s_Context, info);
		s_GraphicsPipelines[id]->SetID(id);
		return ResourceHandle<VulkanGraphicsPipeline>(id, s_GraphicsPipelines[id].get());
	}

	ResourceHandle<VulkanCommandPool> ResourceManager::AllocateCommandPool()
	{
		uint32_t id = s_CommandPools.Add(s_Context);
		s_CommandPools[id]->SetID(id);
		return ResourceHandle<VulkanCommandPool>(id, s_CommandPools[id].get());
	}

	ResourceHandle<VulkanCommandBuffer> ResourceManager::AllocateCommandBuffer(ResourceHandle<VulkanCommandPool> commandPool)
	{
		uint32_t id = s_CommandBuffers.Add(s_Context, commandPool);
		s_CommandBuffers[id]->SetID(id);
		return ResourceHandle<VulkanCommandBuffer>(id, s_CommandBuffers[id].get());
	}

	ResourceHandle<VulkanDescriptorLayout> ResourceManager::AllocateDescriptorLayout(DescriptorType type, ShaderStage shaderStage, uint32_t bindingIndex)
	{
		uint32_t id = s_DescriptorLayouts.Add(s_Context, type, shaderStage, bindingIndex);
		s_DescriptorLayouts[id]->SetID(id);
		return ResourceHandle<VulkanDescriptorLayout>(id, s_DescriptorLayouts[id].get());
	}

	ResourceHandle<VulkanDescriptorBuffer> ResourceManager::AllocateDescriptorBuffer(ResourceHandle<VulkanDescriptorLayout> layout, uint32_t descriptorCount)
	{
		uint32_t id = s_DescriptorBuffers.Add(s_Context, layout, descriptorCount);
		s_DescriptorBuffers[id]->SetID(id);
		return ResourceHandle<VulkanDescriptorBuffer>(id, s_DescriptorBuffers[id].get());
	}

	void ResourceManager::DestroyBuffer(ResourceHandle<VulkanBuffer> handle)
	{
		s_Buffers[handle.m_ID]->Destroy();
		s_Buffers[handle.m_ID]->SetID(-1);
		s_Buffers.Remove(handle.m_ID);
	}

	void ResourceManager::DestroyVertexBuffer(ResourceHandle<VulkanVertexBuffer> handle)
	{
		s_VertexBuffers[handle.m_ID]->Destroy();
		s_VertexBuffers[handle.m_ID]->SetID(-1);
		s_VertexBuffers.Remove(handle.m_ID);
	}

	void ResourceManager::DestroyIndexBuffer(ResourceHandle<VulkanIndexBuffer> handle)
	{
		s_IndexBuffers[handle.m_ID]->Destroy();
		s_IndexBuffers[handle.m_ID]->SetID(-1);
		s_IndexBuffers.Remove(handle.m_ID);
	}

	void ResourceManager::DestroyTexture(ResourceHandle<VulkanTexture> handle)
	{
		s_Textures[handle.m_ID]->Destroy();
		s_Textures[handle.m_ID]->SetID(-1);
		s_Textures.Remove(handle.m_ID);
	}
	void ResourceManager::DestroyTexture(ResourceHandle<VulkanTexture> handle, uint32_t frameIndex)
	{
		if (frameIndex == 0)
		{
			uint32_t id = handle.m_ID;
			auto callback = [id]()
				{
					s_Textures[id]->Destroy();
					s_Textures[id]->SetID(-1);
					s_Textures.Remove(id);
				};
			destroys0.push_back(callback);
		}
		else
		{
			uint32_t id = handle.m_ID;
			auto callback = [id]()
				{
					s_Textures[id]->Destroy();
					s_Textures[id]->SetID(-1);
					s_Textures.Remove(id);
				};
			destroys1.push_back(callback);
		}
	}

	void ResourceManager::DestroyShader(ResourceHandle<VulkanShaderModule> handle)
	{
		s_Shaders[handle.m_ID]->Destroy();
		s_Shaders[handle.m_ID]->SetID(-1);
		s_Shaders.Remove(handle.m_ID);
	}

	void ResourceManager::DestroyGraphicsPipeline(ResourceHandle<VulkanGraphicsPipeline> handle)
	{
		s_GraphicsPipelines[handle.m_ID]->Destroy();
		s_GraphicsPipelines[handle.m_ID]->SetID(-1);
		s_GraphicsPipelines.Remove(handle.m_ID);
	}

	void ResourceManager::DestroyCommandPool(ResourceHandle<VulkanCommandPool> handle)
	{
		s_CommandPools[handle.m_ID]->Destroy();
		s_CommandPools[handle.m_ID]->SetID(-1);
		s_CommandPools.Remove(handle.m_ID);
	}
	void ResourceManager::DestroyCommandBuffer(ResourceHandle<VulkanCommandBuffer> bufferHandle, ResourceHandle<VulkanCommandPool> poolHandle)
	{
		s_CommandBuffers[bufferHandle.m_ID]->Destroy(poolHandle);
		s_CommandBuffers[bufferHandle.m_ID]->SetID(-1);
		s_CommandBuffers.Remove(bufferHandle.m_ID);
	}
	void ResourceManager::DestroyDescriptorLayout(ResourceHandle<VulkanDescriptorLayout> handle)
	{
		s_DescriptorLayouts[handle.m_ID]->Destroy();
		s_DescriptorLayouts[handle.m_ID]->SetID(-1);
		s_DescriptorLayouts.Remove(handle.m_ID);
	}
	void ResourceManager::DestroyDescriptorBuffer(ResourceHandle<VulkanDescriptorBuffer> handle)
	{
		s_DescriptorBuffers[handle.m_ID]->Destroy();
		s_DescriptorBuffers[handle.m_ID]->SetID(-1);
		s_DescriptorBuffers.Remove(handle.m_ID);
	}

	void ResourceManager::FlushDestroys(uint32_t frameIndex)
	{
		if (frameIndex == 0)
		{
			for (const auto& callback : destroys0)
			{
				callback();
			}
			destroys0.clear();
		}
		else
		{
			for (const auto& callback : destroys1)
			{
				callback();
			}
			destroys1.clear();
		}
	}
}