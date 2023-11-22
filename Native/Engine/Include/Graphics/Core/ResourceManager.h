#pragma once
#include "VulkanVertex.h"
#include "VulkanVertexBuffer.h"
#include <DynamicList.h>

namespace Odyssey
{
	class VulkanContext;
	class VulkanVertexBuffer;

	template<typename T>
	struct ResourceHandle
	{
	public:
		ResourceHandle()
		{
			m_ID = -1;
			m_Ptr = nullptr;
		}

		ResourceHandle(uint32_t id, T* ptr)
		{
			m_ID = id;
			m_Ptr = ptr;
		}

	public:
		T* Get() { return m_Ptr; }

	private:
		friend class ResourceManager;
		uint32_t m_ID;
		T* m_Ptr;
	};

	class ResourceManager
	{
	public:
		static void Initialize(std::shared_ptr<VulkanContext> context);

		static ResourceHandle<VulkanVertexBuffer> AllocateVertexBuffer(std::vector<VulkanVertex>& vertices);
		static VulkanVertexBuffer* GetVertexBuffer(ResourceHandle<VulkanVertexBuffer> handle);
		static void DestroyVertexBuffer(ResourceHandle<VulkanVertexBuffer> handle);

	private:
		inline static std::shared_ptr<VulkanContext> m_Context = nullptr;
		inline static DynamicList<VulkanVertexBuffer> m_VertexBuffers;
	};

}