#pragma once
#include "VulkanBuffer.h"

namespace Odyssey
{
	class VulkanUniformBuffer : public VulkanBuffer
	{
	public:
		VulkanUniformBuffer(std::shared_ptr<VulkanContext> context, BufferType bufferType, uint32_t bindingIndex, VkDeviceSize size);

	public:
		VkWriteDescriptorSet GetDescriptorInfo();

	private:
		uint32_t m_BindingIndex;
		VkDescriptorBufferInfo descriptor;
	};
}