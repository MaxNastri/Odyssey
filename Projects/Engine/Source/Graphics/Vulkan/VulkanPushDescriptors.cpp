#include "VulkanPushDescriptors.h"
#include "VulkanUniformBuffer.h"
#include "VulkanTexture.h"

namespace Odyssey
{
	void VulkanPushDescriptors::Add(ResourceHandle<VulkanUniformBuffer> buffer, uint32_t bindingIndex)
	{
		m_WriteDescriptors.push_back(buffer.Get()->GetDescriptorInfo());
		m_WriteDescriptors[m_WriteDescriptors.size() - 1].dstBinding = bindingIndex;
	}

	void VulkanPushDescriptors::Add(ResourceHandle<VulkanTexture> texture, uint32_t bindingIndex)
	{
		m_WriteDescriptors.push_back(texture.Get()->GetDescriptorInfo());
		m_WriteDescriptors[m_WriteDescriptors.size() - 1].dstBinding = bindingIndex;
	}

	void VulkanPushDescriptors::Clear()
	{
		m_WriteDescriptors.clear();
	}
}