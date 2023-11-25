#pragma once
#include "Enums.h"
#include "ResourceHandle.h"
#include "VulkanGlobals.h"

VK_FWD_DECLARE(VkDescriptorSetLayout)

namespace Odyssey
{
	class VulkanBuffer;
	class VulkanContext;
	class VulkanDescriptorLayout;

	class VulkanDescriptorBuffer
	{
	public:
		VulkanDescriptorBuffer(std::shared_ptr<VulkanContext> context);
		void Destroy();

	private:
		std::shared_ptr<VulkanContext> m_Context;
		uint32_t size;
		uint32_t offset;
		ResourceHandle<VulkanBuffer> m_Buffer;
		ResourceHandle<VulkanDescriptorLayout> m_Layout;
		//DescriptorBufferUsage m_Usage;
	};
}