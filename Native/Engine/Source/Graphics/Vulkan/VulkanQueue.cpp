#include "VulkanQueue.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"

namespace Odyssey
{
	VulkanQueue::VulkanQueue(VulkanQueueType type, std::shared_ptr<VulkanContext> context)
	{
		m_Context = context;
		queueType = type;

		vkGetDeviceQueue(context->GetDevice()->GetLogicalDevice(),
			context->GetPhysicalDevice()->GetFamilyIndex(queueType), 0, &queue);
	}
}