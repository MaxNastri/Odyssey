#pragma once
#include "Enums.h"
#include "VulkanTypes.h"
#include "DynamicList.h"
#include "ResourceHandle.h"
#include "VulkanVertex.h"

namespace Odyssey
{
	class Material;
	class Mesh;
	class VulkanBuffer;
	class VulkanContext;
	class VulkanGraphicsPipeline;
	class VulkanIndexBuffer;
	class VulkanShader;
	class VulkanTexture;
	class VulkanVertexBuffer;
	class VulkanCommandPool;
	class VulkanCommandBuffer;
	class VulkanDescriptorBuffer;
	class VulkanDescriptorLayout;

	class ResourceManager
	{
	public:
		static void Initialize(std::shared_ptr<VulkanContext> context);

	public: // Base allocations
		static ResourceHandle<Mesh> AllocateMesh(std::vector<VulkanVertex>& vertices, std::vector<uint32_t>& indices);
		static ResourceHandle<Mesh> AllocateMesh(ResourceHandle<VulkanVertexBuffer> vertexBuffer, ResourceHandle<VulkanIndexBuffer> indexBuffer);
		static ResourceHandle<Material> AllocateMaterial(ResourceHandle<VulkanShader> vertexShader, ResourceHandle<VulkanShader> fragmentShader);

	public: // Vulkan allocations
		static ResourceHandle<VulkanBuffer> AllocateBuffer(BufferType bufferType, uint32_t size);
		static ResourceHandle<VulkanVertexBuffer> AllocateVertexBuffer(std::vector<VulkanVertex>& vertices);
		static ResourceHandle<VulkanIndexBuffer> AllocateIndexBuffer(std::vector<uint32_t>& indices);
		static ResourceHandle<VulkanTexture> AllocateTexture(uint32_t width, uint32_t height);
		static ResourceHandle<VulkanShader> AllocateShader(ShaderType shaderType, const std::string& filename);
		static ResourceHandle<VulkanGraphicsPipeline> AllocateGraphicsPipeline(const VulkanPipelineInfo& info);
		static ResourceHandle<VulkanCommandPool> AllocateCommandPool();
		static ResourceHandle<VulkanCommandBuffer> AllocateCommandBuffer(ResourceHandle<VulkanCommandPool> commandPool);
		static ResourceHandle<VulkanDescriptorLayout> AllocateDescriptorLayout(DescriptorType type, ShaderStage shaderStag, uint32_t bindingIndex);
		static ResourceHandle<VulkanDescriptorBuffer> AllocateDescriptorBuffer(ResourceHandle<VulkanDescriptorLayout> layout, uint32_t descriptorCount);

	public: // Pure destruction
		static void DestroyBuffer(ResourceHandle<VulkanBuffer> handle);
		static void DestroyVertexBuffer(ResourceHandle<VulkanVertexBuffer> handle);
		static void DestroyIndexBuffer(ResourceHandle<VulkanIndexBuffer> handle);
		static void DestroyTexture(ResourceHandle<VulkanTexture> handle);
		static void DestroyShader(ResourceHandle<VulkanShader> handle);
		static void DestroyGraphicsPipeline(ResourceHandle<VulkanGraphicsPipeline> handle);
		static void DestroyCommandPool(ResourceHandle<VulkanCommandPool> handle);
		static void DestroyCommandBuffer(ResourceHandle<VulkanCommandBuffer> bufferHandle, ResourceHandle<VulkanCommandPool> poolHandle);

	private: // Base type members
		inline static DynamicList<Mesh> m_Meshes;
		inline static DynamicList<Material> m_Materials;

	private: // Vulkan members
		inline static std::shared_ptr<VulkanContext> m_Context = nullptr;
		inline static DynamicList<VulkanVertexBuffer> m_VertexBuffers;
		inline static DynamicList<VulkanIndexBuffer> m_IndexBuffers;
		inline static DynamicList<VulkanTexture> m_Textures;
		inline static DynamicList<VulkanShader> m_Shaders;
		inline static DynamicList<VulkanGraphicsPipeline> m_GraphicsPipelines;
		inline static DynamicList<VulkanBuffer> m_Buffers;
		inline static DynamicList<VulkanCommandPool> m_CommandPools;
		inline static DynamicList<VulkanCommandBuffer> m_CommandBuffers;
		inline static DynamicList<VulkanDescriptorLayout> m_DescriptorLayouts;
		inline static DynamicList<VulkanDescriptorBuffer> m_DescriptorBuffers;
	};

}