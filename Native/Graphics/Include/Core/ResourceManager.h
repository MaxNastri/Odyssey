#pragma once
#include "VulkanVertex.h"
#include "VulkanVertexBuffer.h"
#include <queue>

namespace Odyssey
{
	class VulkanContext;
	class VulkanVertexBuffer;

	struct ResourceHandle
	{
	public:
		uint32_t ID;
	};

	template <typename T>
	class ResourceList
	{
	public:
		ResourceList()
		{
			m_Data.resize(initialCapacity);

			for (uint32_t i = 0; i < initialCapacity; i++)
			{
				m_FreeIndices.push(i);
			}
		}

	public:
		T* operator[](int index) { return m_Data[index].get(); }

		template<typename... Args>
		uint32_t Add(Args... params)
		{
			if (m_FreeIndices.size() == 0)
			{
				uint32_t nextIndex = (uint32_t)m_Data.size();

				m_Data.resize(m_Data.size() * 2);

				for (; nextIndex < m_Data.size(); nextIndex++)
				{
					m_FreeIndices.push(nextIndex);
				}
			}

			uint32_t nextIndex = m_FreeIndices.front();
			m_FreeIndices.pop();

			m_Data[nextIndex] = std::make_shared<T>(params...);
			return nextIndex;
		}

		void Free(uint32_t index)
		{
			m_Data[index].reset();
			m_FreeIndices.push(index);
		}

	private:
		const size_t initialCapacity = 64;
		std::vector<std::shared_ptr<T>> m_Data;
		std::queue<uint32_t> m_FreeIndices;
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
		inline static ResourceList<VulkanVertexBuffer> m_VertexBuffers;
		inline static uint32_t m_NextVertexBufferID = 0;
	};

}