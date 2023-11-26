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
		VulkanDescriptorBuffer(std::shared_ptr<VulkanContext> context, ResourceHandle<VulkanDescriptorLayout> layout, uint32_t count);
		void Destroy();

	public:
		void SetUniformBuffer(ResourceHandle<VulkanBuffer> uniformBuffer, uint32_t index);
		
	public:
		ResourceHandle<VulkanBuffer> GetBuffer() { return m_Buffer; }

	private:
		struct Properties
		{
			VkDeviceSize offsetAlignment;
			size_t uniformBufferDescriptorSize;
		};

		std::shared_ptr<VulkanContext> m_Context;
		uint32_t m_Size;
		uint32_t m_Offset;
		ResourceHandle<VulkanBuffer> m_Buffer;
		ResourceHandle<VulkanDescriptorLayout> m_Layout;
		uint32_t m_Count;
		Properties m_Properties;
		//DescriptorBufferUsage m_Usage;
	};
}