#pragma once
#include "VulkanGlobals.h"
#include "Resource.h"

namespace Odyssey
{
	class VulkanPushDescriptors
	{
	public:
		VulkanPushDescriptors() = default;
	public:
		void AddBuffer(ResourceID bufferID, uint32_t bindingIndex);
		void AddTexture(ResourceID textureID, uint32_t bindingIndex);
		void Clear();

	public:
		std::vector<VkWriteDescriptorSet> GetWriteDescriptors() { return m_WriteDescriptors; }
	
	private:
		std::vector<VkWriteDescriptorSet> m_WriteDescriptors;
	};
}