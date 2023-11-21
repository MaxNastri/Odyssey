#pragma once
#include "VulkanVertex.h"
#include "VulkanVertexBuffer.h"
#include <DynamicList.h>

namespace Odyssey
{
	class VulkanContext;
	class VulkanVertexBuffer;

	struct ResourceHandle
	{
	public:
		uint32_t ID;
	};

	class ResourceManager
	{
	public:
		static void Initialize(std::shared_ptr<VulkanContext> context);

		static ResourceHandle AllocateVertexBuffer(std::vector<VulkanVertex>& vertices);
		static VulkanVertexBuffer* GetVertexBuffer(ResourceHandle handle) { return m_VertexBuffers[handle.ID]; }
		static void DestroyVertexBuffer(ResourceHandle handle);
	private:
		inline static std::shared_ptr<VulkanContext> m_Context = nullptr;
		inline static DynamicList<VulkanVertexBuffer> m_VertexBuffers;
		inline static uint32_t m_NextVertexBufferID = 0;
	};

}