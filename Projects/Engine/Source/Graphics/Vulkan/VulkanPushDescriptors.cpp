#include "VulkanPushDescriptors.h"
#include "VulkanUniformBuffer.h"
#include "VulkanStorageBuffer.h"
#include "VulkanTexture.h"
#include "ResourceManager.h"

namespace Odyssey
{
	void VulkanPushDescriptors::AddUniformBuffer(ResourceID bufferID, uint32_t bindingIndex)
	{
		auto buffer = ResourceManager::GetResource<VulkanUniformBuffer>(bufferID);
		m_WriteDescriptors.push_back(buffer->GetDescriptorInfo());
		m_WriteDescriptors[m_WriteDescriptors.size() - 1].dstBinding = bindingIndex;
	}

	void VulkanPushDescriptors::AddStorageBuffer(ResourceID bufferID, uint32_t bindingIndex)
	{
		auto buffer = ResourceManager::GetResource<VulkanStorageBuffer>(bufferID);
		m_WriteDescriptors.push_back(buffer->GetDescriptorInfo());
		m_WriteDescriptors[m_WriteDescriptors.size() - 1].dstBinding = bindingIndex;
	}

	void VulkanPushDescriptors::AddTexture(ResourceID textureID, uint32_t bindingIndex)
	{
		auto texture = ResourceManager::GetResource<VulkanTexture>(textureID);
		m_WriteDescriptors.push_back(texture->GetDescriptorInfo());
		m_WriteDescriptors[m_WriteDescriptors.size() - 1].dstBinding = bindingIndex;
	}

	void VulkanPushDescriptors::Clear()
	{
		m_WriteDescriptors.clear();
	}
}