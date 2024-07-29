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

		ResourceHandle(int32_t id, T* ptr)
		{
			m_ID = id;
			m_Ptr = ptr;
		}

	public:
		operator bool() { return IsValid(); }

	public:
		T* Get() { return m_Ptr; }
		int32_t GetID() { return m_ID; }
		bool IsValid() { return m_ID != -1; }

	private:
		friend class ResourceManager;
		int32_t m_ID = -1;
		T* m_Ptr = nullptr;
	};
}