#pragma once
#include "Enums.h"
#include "VulkanTypes.h"
#include "DynamicList.h"
#include "ResourceHandle.h"
#include "VulkanVertex.h"
#include "VulkanImage.h"

namespace Odyssey
{
	class VulkanBuffer;
	class VulkanContext;
	class VulkanGraphicsPipeline;
	class VulkanIndexBuffer;
	class VulkanShaderModule;
	class VulkanRenderTexture;
	class VulkanVertexBuffer;
	class VulkanCommandPool;
	class VulkanCommandBuffer;
	class VulkanDescriptorBuffer;
	class VulkanDescriptorLayout;
	class VulkanImage;
	class VulkanTextureSampler;

	class ResourceManager
	{
	public:
		static void Initialize(std::shared_ptr<VulkanContext> context);

	public: // Vulkan allocations
		static ResourceHandle<VulkanBuffer> AllocateBuffer(BufferType bufferType, uint32_t size);
		static ResourceHandle<VulkanVertexBuffer> AllocateVertexBuffer(std::vector<VulkanVertex>& vertices);
		static ResourceHandle<VulkanIndexBuffer> AllocateIndexBuffer(std::vector<uint32_t>& indices);
		static ResourceHandle<VulkanRenderTexture> AllocateTexture(uint32_t width, uint32_t height);
		static ResourceHandle<VulkanRenderTexture> AllocateRenderTexture(ResourceHandle<VulkanImage> imageHandle, TextureFormat format);
		static ResourceHandle<VulkanRenderTexture> AllocateRenderTexture(uint32_t width, uint32_t height, TextureFormat format);
		static ResourceHandle<VulkanShaderModule> AllocateShaderModule(ShaderType shaderType, const std::string& filename);
		static ResourceHandle<VulkanGraphicsPipeline> AllocateGraphicsPipeline(const VulkanPipelineInfo& info);
		static ResourceHandle<VulkanCommandPool> AllocateCommandPool();
		static ResourceHandle<VulkanCommandBuffer> AllocateCommandBuffer(ResourceHandle<VulkanCommandPool> commandPool);
		static ResourceHandle<VulkanDescriptorLayout> AllocateDescriptorLayout(DescriptorType type, ShaderStage shaderStag, uint32_t bindingIndex);
		static ResourceHandle<VulkanDescriptorBuffer> AllocateDescriptorBuffer(ResourceHandle<VulkanDescriptorLayout> layout, uint32_t descriptorCount);
		static ResourceHandle<VulkanImage> AllocateImage(const VulkanImageDescription& imageDescription);
		static ResourceHandle<VulkanImage> AllocateImage(VkImage image, uint32_t width, uint32_t height, VkFormat format);
		static ResourceHandle<VulkanTextureSampler> AllocateSampler();

	public: // Pure destruction
		static void DestroyBuffer(ResourceHandle<VulkanBuffer> handle);
		static void DestroyVertexBuffer(ResourceHandle<VulkanVertexBuffer> handle);
		static void DestroyIndexBuffer(ResourceHandle<VulkanIndexBuffer> handle);
		static void DestroyTexture(ResourceHandle<VulkanRenderTexture> handle);
		static void DestroyShader(ResourceHandle<VulkanShaderModule> handle);
		static void DestroyGraphicsPipeline(ResourceHandle<VulkanGraphicsPipeline> handle);
		static void DestroyCommandPool(ResourceHandle<VulkanCommandPool> handle);
		static void DestroyCommandBuffer(ResourceHandle<VulkanCommandBuffer> bufferHandle, ResourceHandle<VulkanCommandPool> poolHandle);
		static void DestroyDescriptorLayout(ResourceHandle<VulkanDescriptorLayout> handle);
		static void DestroyDescriptorBuffer(ResourceHandle<VulkanDescriptorBuffer> handle);
		static void DestroyImage(ResourceHandle<VulkanImage> handle);
		static void DestroySampler(ResourceHandle<VulkanTextureSampler> handle);

	public:
		static void Flush();

	private: // Vulkan members
		inline static std::shared_ptr<VulkanContext> s_Context = nullptr;
		inline static DynamicList<VulkanVertexBuffer> s_VertexBuffers;
		inline static DynamicList<VulkanIndexBuffer> s_IndexBuffers;
		inline static DynamicList<VulkanRenderTexture> s_Textures;
		inline static DynamicList<VulkanShaderModule> s_Shaders;
		inline static DynamicList<VulkanGraphicsPipeline> s_GraphicsPipelines;
		inline static DynamicList<VulkanBuffer> s_Buffers;
		inline static DynamicList<VulkanCommandPool> s_CommandPools;
		inline static DynamicList<VulkanCommandBuffer> s_CommandBuffers;
		inline static DynamicList<VulkanDescriptorLayout> s_DescriptorLayouts;
		inline static DynamicList<VulkanDescriptorBuffer> s_DescriptorBuffers;
		inline static DynamicList<VulkanImage> s_Images;
		inline static DynamicList<VulkanTextureSampler> s_Samplers;

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