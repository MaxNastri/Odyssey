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

		ResourceHandle(uint64_t id, T* ptr)
		{
			m_ID = id;
			m_Ptr = ptr;
		}

	public:
		operator bool() { return IsValid(); }
		T* operator ->() { return m_Ptr; }

	public:
		T* Get() { return m_Ptr; }
		uint64_t GetID() { return m_ID; }
		bool IsValid() { return m_ID != (uint64_t)(-1); }

	private:
		friend class ResourceManager;
		uint64_t m_ID = (uint64_t)(-1);
		T* m_Ptr = nullptr;
	};
}