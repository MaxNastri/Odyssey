#pragma once
#include "Enums.h"
#include "VulkanTypes.h"
#include "DynamicList.h"
#include "ResourceHandle.h"
#include "VulkanVertex.h"

namespace Odyssey
{
	class VulkanBuffer;
	class VulkanContext;
	class VulkanGraphicsPipeline;
	class VulkanIndexBuffer;
	class VulkanShader;
	class VulkanTexture;
	class VulkanVertexBuffer;

	class ResourceManager
	{
	public:
		static void Initialize(std::shared_ptr<VulkanContext> context);

	public: // Allocations
		static ResourceHandle<VulkanBuffer> AllocateBuffer(BufferType bufferType, uint32_t size);
		static ResourceHandle<VulkanVertexBuffer> AllocateVertexBuffer(std::vector<VulkanVertex>& vertices);
		static ResourceHandle<VulkanIndexBuffer> AllocateIndexBuffer(std::vector<uint32_t>& indices);
		static ResourceHandle<VulkanTexture> AllocateTexture(uint32_t width, uint32_t height);
		static ResourceHandle<VulkanShader> AllocateShader(ShaderType shaderType, const std::string& filename);
		static ResourceHandle<VulkanGraphicsPipeline> AllocateGraphicsPipeline(const VulkanPipelineInfo& info);

	public: // Pure destruction
		static void DestroyBuffer(ResourceHandle<VulkanBuffer> handle);
		static void DestroyVertexBuffer(ResourceHandle<VulkanVertexBuffer> handle);
		static void DestroyIndexBuffer(ResourceHandle<VulkanIndexBuffer> handle);
		static void DestroyTexture(ResourceHandle<VulkanTexture> handle);
		static void DestroyShader(ResourceHandle<VulkanShader> handle);
		static void DestroyGraphicsPipeline(ResourceHandle<VulkanGraphicsPipeline> handle);

	private:
		inline static std::shared_ptr<VulkanContext> m_Context = nullptr;
		inline static DynamicList<VulkanVertexBuffer> m_VertexBuffers;
		inline static DynamicList<VulkanIndexBuffer> m_IndexBuffers;
		inline static DynamicList<VulkanTexture> m_Textures;
		inline static DynamicList<VulkanShader> m_Shaders;
		inline static DynamicList<VulkanGraphicsPipeline> m_GraphicsPipelines;
		inline static DynamicList<VulkanBuffer> m_Buffers;
	};

}