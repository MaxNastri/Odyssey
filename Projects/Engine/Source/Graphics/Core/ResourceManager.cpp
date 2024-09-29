#include "ResourceManager.h"
#include "Resource.h"
#include "VulkanContext.h"

namespace Odyssey
{
	void ResourceManager::Initialize(std::shared_ptr<VulkanContext> context)
	{
		s_Context = context;
	}

	void ResourceManager::Flush()
	{
		if (s_PendingDestroys.size() > 0)
		{
			for (int32_t i = (int32_t)s_PendingDestroys.size() - 1; i >= 0; i--)
			{
				s_PendingDestroys[i].Execute();
			}

			s_PendingDestroys.clear();
		}
	}
}