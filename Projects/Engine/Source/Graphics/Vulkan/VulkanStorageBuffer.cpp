#include "VulkanStorageBuffer.h"

namespace Odyssey
{
	VulkanStorageBuffer::VulkanStorageBuffer(ResourceID id, std::shared_ptr<VulkanContext> context, BufferType bufferType, uint32_t bindingIndex, VkDeviceSize size)
		: VulkanBuffer(id, context, bufferType, size)
	{
		m_BindingIndex = bindingIndex;

		descriptor.buffer = buffer;
		descriptor.range = VK_WHOLE_SIZE;
		descriptor.offset = 0;
	}
	VkWriteDescriptorSet VulkanStorageBuffer::GetDescriptorInfo()
	{
		VkWriteDescriptorSet writeSet{};
		writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeSet.dstSet = 0;
		writeSet.dstBinding = m_BindingIndex;
		writeSet.descriptorCount = 1;
		writeSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeSet.pBufferInfo = &descriptor;
		return writeSet;
	}
}