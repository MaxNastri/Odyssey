#pragma once
#include <limits>

namespace Odyssey
{
	class ResourceManager;

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
		uint32_t GetID() { return m_ID; }
		bool IsValid() { return m_ID != std::numeric_limits<uint32_t>::max(); }

	private:
		friend class ResourceManager;
		uint32_t m_ID = std::numeric_limits<uint32_t>::max();
		T* m_Ptr = nullptr;
	};
}