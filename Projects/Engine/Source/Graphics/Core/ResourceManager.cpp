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
		auto destroys = s_PendingDestroys;

		if (destroys.size() > 0)
		{
			for (int32_t i = (int32_t)destroys.size() - 1; i >= 0; i--)
			{
				destroys[i].Execute();
				s_PendingDestroys.erase(s_PendingDestroys.begin() + i);
			}
		}
	}
}