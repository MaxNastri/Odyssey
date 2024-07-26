#include "ResourceManager.h"
#include "Resource.h"
#include "VulkanContext.h"
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanRenderTexture.h"
#include "VulkanShaderModule.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanBuffer.h"
#include "VulkanUniformBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandPool.h"
#include "VulkanDescriptorLayout.h"
#include "VulkanImage.h"
#include "VulkanTextureSampler.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSet.h"
#include "VulkanTexture.h"

namespace Odyssey
{
	void ResourceManager::Initialize(std::shared_ptr<VulkanContext> context)
	{
		s_Context = context;
	}

	ResourceHandle<VulkanBuffer> ResourceManager::AllocateBuffer(BufferType bufferType, uint32_t size)
	{
		uint32_t id = s_Resources.Add<VulkanBuffer>(s_Context, bufferType, size);
		s_Resources[id]->SetID(id);

		std::shared_ptr<VulkanBuffer> buffer = s_Resources.Get<VulkanBuffer>(id);
		return ResourceHandle<VulkanBuffer>(id, buffer.get());
	}

	ResourceHandle<VulkanUniformBuffer> ResourceManager::AllocateUniformBuffer(BufferType bufferType, uint32_t bindingIndex, uint32_t size)
	{
		uint32_t id = s_Resources.Add<VulkanUniformBuffer>(s_Context, bufferType, bindingIndex, size);
		s_Resources[id]->SetID(id);

		std::shared_ptr<VulkanUniformBuffer> buffer = s_Resources.Get<VulkanUniformBuffer>(id);
		return ResourceHandle<VulkanUniformBuffer>(id, buffer.get());
	}

	ResourceHandle<VulkanVertexBuffer> ResourceManager::AllocateVertexBuffer(std::vector<VulkanVertex>& vertices)
	{
		uint32_t id = s_Resources.Add<VulkanVertexBuffer>(s_Context, vertices);
		s_Resources[id]->SetID(id);

		std::shared_ptr<VulkanVertexBuffer> buffer = s_Resources.Get<VulkanVertexBuffer>(id);
		return ResourceHandle<VulkanVertexBuffer>(id, buffer.get());
	}

	ResourceHandle<VulkanIndexBuffer> ResourceManager::AllocateIndexBuffer(std::vector<uint32_t>& indices)
	{
		uint32_t id = s_Resources.Add<VulkanIndexBuffer>(s_Context, indices);
		s_Resources[id]->SetID(id);

		std::shared_ptr<VulkanIndexBuffer> buffer = s_Resources.Get<VulkanIndexBuffer>(id);
		return ResourceHandle<VulkanIndexBuffer>(id, buffer.get());
	}

	ResourceHandle<VulkanRenderTexture> ResourceManager::AllocateRenderTexture(uint32_t width, uint32_t height)
	{
		uint32_t id = s_Resources.Add<VulkanRenderTexture>(s_Context, width, height);
		s_Resources[id]->SetID(id);

		std::shared_ptr<VulkanRenderTexture> texture = s_Resources.Get<VulkanRenderTexture>(id);
		return ResourceHandle<VulkanRenderTexture>(id, texture.get());
	}

	ResourceHandle<VulkanRenderTexture> ResourceManager::AllocateRenderTexture(ResourceHandle<VulkanImage> imageHandle, TextureFormat format)
	{
		uint32_t id = s_Resources.Add<VulkanRenderTexture>(s_Context, imageHandle, format);
		s_Resources[id]->SetID(id);

		std::shared_ptr<VulkanRenderTexture> texture = s_Resources.Get<VulkanRenderTexture>(id);
		return ResourceHandle<VulkanRenderTexture>(id, texture.get());
	}

	ResourceHandle<VulkanRenderTexture> ResourceManager::AllocateRenderTexture(uint32_t width, uint32_t height, TextureFormat format)
	{
		uint32_t id = s_Resources.Add<VulkanRenderTexture>(s_Context, width, height, format);
		s_Resources[id]->SetID(id);

		std::shared_ptr<VulkanRenderTexture> texture = s_Resources.Get<VulkanRenderTexture>(id);
		return ResourceHandle<VulkanRenderTexture>(id, texture.get());
	}

	ResourceHandle<VulkanShaderModule> ResourceManager::AllocateShaderModule(ShaderType shaderType, BinaryBuffer& codeBuffer)
	{
		uint32_t id = s_Resources.Add<VulkanShaderModule>(s_Context, shaderType, codeBuffer);
		s_Resources[id]->SetID(id);

		std::shared_ptr<VulkanShaderModule> shaderModule = s_Resources.Get<VulkanShaderModule>(id);
		return ResourceHandle<VulkanShaderModule>(id, shaderModule.get());
	}

	ResourceHandle<VulkanGraphicsPipeline> ResourceManager::AllocateGraphicsPipeline(const VulkanPipelineInfo& info)
	{
		uint32_t id = s_Resources.Add<VulkanGraphicsPipeline>(s_Context, info);
		s_Resources[id]->SetID(id);

		std::shared_ptr<VulkanGraphicsPipeline> pipeline = s_Resources.Get<VulkanGraphicsPipeline>(id);
		return ResourceHandle<VulkanGraphicsPipeline>(id, pipeline.get());
	}

	ResourceHandle<VulkanCommandPool> ResourceManager::AllocateCommandPool()
	{
		uint32_t id = s_Resources.Add<VulkanCommandPool>(s_Context);
		s_Resources[id]->SetID(id);

		std::shared_ptr<VulkanCommandPool> pool = s_Resources.Get<VulkanCommandPool>(id);
		return ResourceHandle<VulkanCommandPool>(id, pool.get());
	}

	ResourceHandle<VulkanCommandBuffer> ResourceManager::AllocateCommandBuffer(ResourceHandle<VulkanCommandPool> commandPool)
	{
		uint32_t id = s_Resources.Add<VulkanCommandBuffer>(s_Context, commandPool);
		s_Resources[id]->SetID(id);

		std::shared_ptr<VulkanCommandBuffer> buffer = s_Resources.Get<VulkanCommandBuffer>(id);
		return ResourceHandle<VulkanCommandBuffer>(id, buffer.get());
	}

	ResourceHandle<VulkanDescriptorLayout> ResourceManager::AllocateDescriptorLayout(DescriptorType type, ShaderStage shaderStage, uint32_t bindingIndex)
	{
		uint32_t id = s_Resources.Add<VulkanDescriptorLayout>(s_Context, type, shaderStage, bindingIndex);
		s_Resources[id]->SetID(id);

		std::shared_ptr<VulkanDescriptorLayout> layout = s_Resources.Get<VulkanDescriptorLayout>(id);
		return ResourceHandle<VulkanDescriptorLayout>(id, layout.get());
	}

	ResourceHandle<VulkanDescriptorPool> ResourceManager::AllocateDescriptorPool(DescriptorType poolType, uint32_t setCount, uint32_t maxSets)
	{
		uint32_t id = s_Resources.Add<VulkanDescriptorPool>(s_Context, poolType, setCount, maxSets);
		s_Resources[id]->SetID(id);

		std::shared_ptr<VulkanDescriptorPool> pool = s_Resources.Get<VulkanDescriptorPool>(id);
		return ResourceHandle<VulkanDescriptorPool>(id, pool.get());
	}

	ResourceHandle<VulkanDescriptorSet> ResourceManager::AllocateDescriptorSet(DescriptorType descriptorType, ResourceHandle<VulkanDescriptorPool> pool, ResourceHandle<VulkanDescriptorLayout> layout, uint32_t count)
	{
		uint32_t id = s_Resources.Add<VulkanDescriptorSet>(s_Context, descriptorType, pool, layout, count);
		s_Resources[id]->SetID(id);

		std::shared_ptr<VulkanDescriptorSet> descriptorSet = s_Resources.Get<VulkanDescriptorSet>(id);
		return ResourceHandle<VulkanDescriptorSet>(id, descriptorSet.get());
	}

	ResourceHandle<VulkanImage> ResourceManager::AllocateImage(const VulkanImageDescription& imageDescription)
	{
		uint32_t id = s_Resources.Add<VulkanImage>(s_Context, imageDescription);
		s_Resources[id]->SetID(id);

		std::shared_ptr<VulkanImage> image = s_Resources.Get<VulkanImage>(id);
		return ResourceHandle<VulkanImage>(id, image.get());
	}

	ResourceHandle<VulkanImage> ResourceManager::AllocateImage(VkImage vkImage, uint32_t width, uint32_t height, uint32_t channels, VkFormat format)
	{
		uint32_t id = s_Resources.Add<VulkanImage>(s_Context, vkImage, width, height, channels, format);
		s_Resources[id]->SetID(id);

		std::shared_ptr<VulkanImage> image = s_Resources.Get<VulkanImage>(id);
		return ResourceHandle<VulkanImage>(id, image.get());
	}

	ResourceHandle<VulkanTextureSampler> ResourceManager::AllocateSampler()
	{
		uint32_t id = s_Resources.Add<VulkanTextureSampler>(s_Context);
		s_Resources[id]->SetID(id);

		std::shared_ptr<VulkanTextureSampler> sampler = s_Resources.Get<VulkanTextureSampler>(id);
		return ResourceHandle<VulkanTextureSampler>(id, sampler.get());
	}

	ResourceHandle<VulkanTexture> ResourceManager::AllocateTexture(VulkanImageDescription imageDesc, const void* pixelData)
	{
		uint32_t id = s_Resources.Add<VulkanTexture>(imageDesc, pixelData);
		s_Resources[id]->SetID(id);

		std::shared_ptr<VulkanTexture> texture = s_Resources.Get<VulkanTexture>(id);
		return ResourceHandle<VulkanTexture>(id, texture.get());
	}

	void ResourceManager::DestroyBuffer(ResourceHandle<VulkanBuffer> handle)
	{
		if (std::shared_ptr<VulkanBuffer> buffer = s_Resources.Get<VulkanBuffer>(handle.m_ID))
		{
			buffer->Destroy();
			buffer->SetID(-1);
			s_Resources.Remove(handle.m_ID);
		}
	}

	void ResourceManager::DestroyUniformBuffer(ResourceHandle<VulkanUniformBuffer> handle)
	{
		uint32_t id = handle.m_ID;
		auto func = [](uint32_t id)
			{
				if (std::shared_ptr<VulkanUniformBuffer> buffer = s_Resources.Get<VulkanUniformBuffer>(id))
				{
					buffer->Destroy();
					buffer->SetID(-1);
					s_Resources.Remove(id);
				}
			};
		s_PendingDestroys.push_back({ id, func });
	}

	void ResourceManager::DestroyVertexBuffer(ResourceHandle<VulkanVertexBuffer> handle)
	{
		if (std::shared_ptr<VulkanVertexBuffer> buffer = s_Resources.Get<VulkanVertexBuffer>(handle.m_ID))
		{
			buffer->Destroy();
			buffer->SetID(-1);
			s_Resources.Remove(handle.m_ID);
		}
	}

	void ResourceManager::DestroyIndexBuffer(ResourceHandle<VulkanIndexBuffer> handle)
	{
		if (std::shared_ptr<VulkanIndexBuffer> buffer = s_Resources.Get<VulkanIndexBuffer>(handle.m_ID))
		{
			buffer->Destroy();
			buffer->SetID(-1);
			s_Resources.Remove(handle.m_ID);
		}
	}

	void ResourceManager::DestroyRenderTexture(ResourceHandle<VulkanRenderTexture> handle)
	{
		uint32_t id = handle.m_ID;
		auto func = [](uint32_t id)
			{
				if (std::shared_ptr<VulkanRenderTexture> renderTexture = s_Resources.Get<VulkanRenderTexture>(id))
				{
					renderTexture->Destroy();
					renderTexture->SetID(-1);
					s_Resources.Remove(id);
				}
			};
		s_PendingDestroys.push_back({ id, func });
	}

	void ResourceManager::DestroyShader(ResourceHandle<VulkanShaderModule> handle)
	{
		if (std::shared_ptr<VulkanShaderModule> shaderModule = s_Resources.Get<VulkanShaderModule>(handle.m_ID))
		{
			shaderModule->Destroy();
			shaderModule->SetID(-1);
			s_Resources.Remove(handle.m_ID);
		}
	}

	void ResourceManager::DestroyGraphicsPipeline(ResourceHandle<VulkanGraphicsPipeline> handle)
	{
		if (std::shared_ptr<VulkanGraphicsPipeline> pipeline = s_Resources.Get<VulkanGraphicsPipeline>(handle.m_ID))
		{
			pipeline->Destroy();
			pipeline->SetID(-1);
			s_Resources.Remove(handle.m_ID);
		}
	}

	void ResourceManager::DestroyCommandPool(ResourceHandle<VulkanCommandPool> handle)
	{
		if (std::shared_ptr<VulkanCommandPool> pool = s_Resources.Get<VulkanCommandPool>(handle.m_ID))
		{
			pool->Destroy();
			pool->SetID(-1);
			s_Resources.Remove(handle.m_ID);
		}
	}
	void ResourceManager::DestroyCommandBuffer(ResourceHandle<VulkanCommandBuffer> bufferHandle, ResourceHandle<VulkanCommandPool> poolHandle)
	{
		if (std::shared_ptr<VulkanCommandBuffer> buffer = s_Resources.Get<VulkanCommandBuffer>(bufferHandle.m_ID))
		{
			buffer->Destroy(poolHandle);
			buffer->SetID(-1);
			s_Resources.Remove(bufferHandle.m_ID);
		}
	}
	void ResourceManager::DestroyDescriptorLayout(ResourceHandle<VulkanDescriptorLayout> handle)
	{
		if (std::shared_ptr<VulkanDescriptorLayout> layout = s_Resources.Get<VulkanDescriptorLayout>(handle.m_ID))
		{
			layout->Destroy();
			layout->SetID(-1);
			s_Resources.Remove(handle.m_ID);
		}
	}
	void ResourceManager::DestroyImage(ResourceHandle<VulkanImage> handle)
	{
		uint32_t id = handle.m_ID;
		auto func = [](uint32_t id)
			{
				if (std::shared_ptr<VulkanImage> image = s_Resources.Get<VulkanImage>(id))
				{
					image->Destroy();
					image->SetID(-1);
					s_Resources.Remove(id);
				}
			};
		s_PendingDestroys.push_back({ id, func });
	}

	void ResourceManager::DestroySampler(ResourceHandle<VulkanTextureSampler> handle)
	{
		uint32_t id = handle.m_ID;
		auto func = [](uint32_t id)
			{
				if (std::shared_ptr<VulkanTextureSampler> sampler = s_Resources.Get<VulkanTextureSampler>(id))
				{
					sampler->Destroy();
					sampler->SetID(-1);
					s_Resources.Remove(id);
				}
			};
		s_PendingDestroys.push_back({ id, func });
	}

	void ResourceManager::DestroyDescriptorPool(ResourceHandle<VulkanDescriptorPool> handle)
	{
		uint32_t id = handle.m_ID;
		auto func = [](uint32_t id)
			{
				if (std::shared_ptr<VulkanDescriptorPool> pool = s_Resources.Get<VulkanDescriptorPool>(id))
				{
					pool->Destroy();
					pool->SetID(-1);
					s_Resources.Remove(id);
				}
			};
		s_PendingDestroys.push_back({ id, func });
	}

	void ResourceManager::DestroyDescriptorSet(ResourceHandle<VulkanDescriptorSet> handle)
	{
		uint32_t id = handle.m_ID;
		auto func = [](uint32_t id)
			{
				if (std::shared_ptr<VulkanDescriptorSet> descriptorSet = s_Resources.Get<VulkanDescriptorSet>(id))
				{
					descriptorSet->SetID(-1);
					s_Resources.Remove(id);
				}
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