#pragma once
#include "VulkanGlobals.h"
#include "ResourceHandle.h"

namespace Odyssey
{
	class VulkanUniformBuffer;
	class VulkanTexture;

	class VulkanPushDescriptors
	{
	public:
		VulkanPushDescriptors() = default;
	public:
		void Add(ResourceHandle<VulkanUniformBuffer> buffer, uint32_t bindingIndex);
		void Add(ResourceHandle<VulkanTexture> texture, uint32_t bindingIndex);
		void Clear();

	public:
		std::vector<VkWriteDescriptorSet> GetWriteDescriptors() { return m_WriteDescriptors; }
	private:
		std::vector<VkWriteDescriptorSet> m_WriteDescriptors;
	};
}