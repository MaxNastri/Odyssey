#pragma once
#include "Enums.h"
#include "VulkanTypes.h"
#include "DynamicList.h"
#include "ResourceHandle.h"
#include "VulkanVertex.h"
#include "VulkanImage.h"

namespace Odyssey
{
	class Resource;
	class VulkanBuffer;
	class VulkanUniformBuffer;
	class VulkanContext;
	class VulkanGraphicsPipeline;
	class VulkanIndexBuffer;
	class VulkanShaderModule;
	class VulkanRenderTexture;
	class VulkanVertexBuffer;
	class VulkanCommandPool;
	class VulkanCommandBuffer;
	class VulkanDescriptorLayout;
	class VulkanImage;
	class VulkanTextureSampler;
	class VulkanDescriptorPool;
	class VulkanDescriptorSet;
	class VulkanTexture;

	class ResourceManager
	{
	public:
		static void Initialize(std::shared_ptr<VulkanContext> context);

	public: // Vulkan allocations
		static ResourceHandle<VulkanBuffer> AllocateBuffer(BufferType bufferType, uint32_t size);
		static ResourceHandle<VulkanUniformBuffer> AllocateUniformBuffer(BufferType bufferType, uint32_t bindingIndex, uint32_t size);
		static ResourceHandle<VulkanVertexBuffer> AllocateVertexBuffer(std::vector<VulkanVertex>& vertices);
		static ResourceHandle<VulkanIndexBuffer> AllocateIndexBuffer(std::vector<uint32_t>& indices);
		static ResourceHandle<VulkanRenderTexture> AllocateRenderTexture(uint32_t width, uint32_t height);
		static ResourceHandle<VulkanRenderTexture> AllocateRenderTexture(ResourceHandle<VulkanImage> imageHandle, TextureFormat format);
		static ResourceHandle<VulkanRenderTexture> AllocateRenderTexture(uint32_t width, uint32_t height, TextureFormat format);
		static ResourceHandle<VulkanShaderModule> AllocateShaderModule(ShaderType shaderType, const std::filesystem::path& filename);
		static ResourceHandle<VulkanGraphicsPipeline> AllocateGraphicsPipeline(const VulkanPipelineInfo& info);
		static ResourceHandle<VulkanCommandPool> AllocateCommandPool();
		static ResourceHandle<VulkanCommandBuffer> AllocateCommandBuffer(ResourceHandle<VulkanCommandPool> commandPool);
		static ResourceHandle<VulkanDescriptorLayout> AllocateDescriptorLayout(DescriptorType type, ShaderStage shaderStage, uint32_t bindingIndex);
		static ResourceHandle<VulkanDescriptorPool> AllocateDescriptorPool(DescriptorType poolType, uint32_t setCount, uint32_t maxSets);
		static ResourceHandle<VulkanDescriptorSet> AllocateDescriptorSet(DescriptorType descriptorType, ResourceHandle<VulkanDescriptorPool> pool, ResourceHandle<VulkanDescriptorLayout> layout, uint32_t count);
		static ResourceHandle<VulkanImage> AllocateImage(const VulkanImageDescription& imageDescription);
		static ResourceHandle<VulkanImage> AllocateImage(VkImage vkImage, uint32_t width, uint32_t height, VkFormat format);
		static ResourceHandle<VulkanTextureSampler> AllocateSampler();
		static ResourceHandle<VulkanTexture> AllocateTexture(VulkanImageDescription imageDesc, const void* pixelData);

	public: // Pure destruction
		static void DestroyBuffer(ResourceHandle<VulkanBuffer> handle);
		static void DestroyUniformBuffer(ResourceHandle<VulkanUniformBuffer> handle);
		static void DestroyVertexBuffer(ResourceHandle<VulkanVertexBuffer> handle);
		static void DestroyIndexBuffer(ResourceHandle<VulkanIndexBuffer> handle);
		static void DestroyRenderTexture(ResourceHandle<VulkanRenderTexture> handle);
		static void DestroyShader(ResourceHandle<VulkanShaderModule> handle);
		static void DestroyGraphicsPipeline(ResourceHandle<VulkanGraphicsPipeline> handle);
		static void DestroyCommandPool(ResourceHandle<VulkanCommandPool> handle);
		static void DestroyCommandBuffer(ResourceHandle<VulkanCommandBuffer> bufferHandle, ResourceHandle<VulkanCommandPool> poolHandle);
		static void DestroyDescriptorLayout(ResourceHandle<VulkanDescriptorLayout> handle);
		static void DestroyImage(ResourceHandle<VulkanImage> handle);
		static void DestroySampler(ResourceHandle<VulkanTextureSampler> handle);
		static void DestroyDescriptorPool(ResourceHandle<VulkanDescriptorPool> handle);
		static void DestroyDescriptorSet(ResourceHandle<VulkanDescriptorSet> handle);
		static void DestroyTexture(ResourceHandle<VulkanTexture> handle);

	public:
		static void Flush();

	private: // Vulkan members
		inline static std::shared_ptr<VulkanContext> s_Context = nullptr;
		inline static DynamicList<Resource> s_Resources;

	private:
		struct ResourceDeallocation
		{
			uint32_t ID;
			std::function<void(uint32_t)> Func;

			void Execute()
			{
				Func(ID);
			}
		};

		inline static std::vector<ResourceDeallocation> s_PendingDestroys;
	};

}