#include "ResourceManager.h"
#include "Resource.h"
#include "VulkanContext.h"
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanRenderTexture.h"
#include "VulkanShaderModule.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandPool.h"
#include "VulkanDescriptorLayout.h"
#include "VulkanDescriptorBuffer.h"
#include "VulkanImage.h"
#include "VulkanTextureSampler.h"

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

	ResourceHandle<VulkanRenderTexture> ResourceManager::AllocateTexture(uint32_t width, uint32_t height)
	{
		uint32_t id = s_Textures.Add(s_Context, width, height);
		s_Textures[id]->SetID(id);
		return ResourceHandle<VulkanRenderTexture>(id, s_Textures[id].get());
	}

	ResourceHandle<VulkanRenderTexture> ResourceManager::AllocateRenderTexture(ResourceHandle<VulkanImage> imageHandle, TextureFormat format)
	{
		uint32_t id = s_Textures.Add(s_Context, imageHandle, format);
		s_Textures[id]->SetID(id);
		return ResourceHandle<VulkanRenderTexture>(id, s_Textures[id].get());
	}

	ResourceHandle<VulkanRenderTexture> ResourceManager::AllocateRenderTexture(uint32_t width, uint32_t height, TextureFormat format)
	{
		uint32_t id = s_Textures.Add(s_Context, width, height, format);
		s_Textures[id]->SetID(id);
		return ResourceHandle<VulkanRenderTexture>(id, s_Textures[id].get());
	}

	ResourceHandle<VulkanShaderModule> ResourceManager::AllocateShaderModule(ShaderType shaderType, const std::filesystem::path& filename)
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

	ResourceHandle<VulkanImage> ResourceManager::AllocateImage(const VulkanImageDescription& imageDescription)
	{
		uint32_t id = s_Images.Add(s_Context, imageDescription);
		s_Images[id]->SetID(id);
		return ResourceHandle<VulkanImage>(id, s_Images[id].get());
	}

	ResourceHandle<VulkanImage> ResourceManager::AllocateImage(VkImage image, uint32_t width, uint32_t height, VkFormat format)
	{
		uint32_t id = s_Images.Add(s_Context, image, width, height, format);
		s_Images[id]->SetID(id);
		return ResourceHandle<VulkanImage>(id, s_Images[id].get());
	}

	ResourceHandle<VulkanTextureSampler> ResourceManager::AllocateSampler()
	{
		uint32_t id = s_Samplers.Add(s_Context);
		s_Samplers[id]->SetID(id);
		return ResourceHandle<VulkanTextureSampler>(id, s_Samplers[id].get());
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

	void ResourceManager::DestroyTexture(ResourceHandle<VulkanRenderTexture> handle)
	{
		uint32_t id = handle.m_ID;
		auto func = [](uint32_t id)
			{
				s_Textures[id]->Destroy();
				s_Textures[id]->SetID(-1);
				s_Textures.Remove(id);
			};
		s_PendingDestroys.push_back({ id, func });
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

	void ResourceManager::DestroyImage(ResourceHandle<VulkanImage> handle)
	{
		uint32_t id = handle.m_ID;
		auto func = [](uint32_t id)
			{
				s_Images[id]->Destroy();
				s_Images[id]->SetID(-1);
				s_Images.Remove(id);
			};
		s_PendingDestroys.push_back({ id, func });
	}

	void ResourceManager::DestroySampler(ResourceHandle<VulkanTextureSampler> handle)
	{
		uint32_t id = handle.m_ID;
		auto func = [](uint32_t id)
			{
				s_Samplers[id]->Destroy();
				s_Samplers[id]->SetID(-1);
				s_Samplers.Remove(id);
			};
		s_PendingDestroys.push_back({ id, func });
	}

	void ResourceManager::Flush()
	{
		if (s_PendingDestroys.size() > 0)
		{
			for (int32_t i = (int32_t)s_PendingDestroys.size() - 1; i >= 0; i--)
			{
				s_PendingDestroys[i].Execute();
			}

			s_PendingDestroys.clear();
		}
	}
}