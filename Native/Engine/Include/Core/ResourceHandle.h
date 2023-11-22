#pragma once

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

	private:
		friend class ResourceManager;
		uint32_t m_ID;
		T* m_Ptr;
	};
}