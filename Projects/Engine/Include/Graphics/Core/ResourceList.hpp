#pragma once
#include "Resource.h"
#include "FreeList.hpp"

namespace Odyssey
{
	class ResourceList
	{
	public:
		ResourceList() = default;

	public:
		template<typename T, typename... Args>
		ResourceID Add(Args... params)
		{
			// Add the resource to the free list
			ResourceID resourceID = m_Resources.Peek();
			m_Resources.Add<T>(resourceID, std::forward<Args>(params)...);

			return resourceID;
		}

		std::shared_ptr<Resource> Get(uint64_t id)
		{
			return m_Resources[id];
		}

		template<typename T>
		std::shared_ptr<T> Get(uint64_t id)
		{
			if (std::shared_ptr<Resource> value = Get(id))
				return std::static_pointer_cast<T>(value);

			return nullptr;
		}

		void Remove(uint64_t id)
		{
			m_Resources.Remove(id);
		}

	private:
		FreeList<Resource> m_Resources;
	};
}