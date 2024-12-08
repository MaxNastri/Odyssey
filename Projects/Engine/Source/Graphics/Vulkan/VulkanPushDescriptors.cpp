#include "VulkanPushDescriptors.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"
#include "ResourceManager.h"
#include "VulkanRenderTexture.h"

namespace Odyssey
{
	void VulkanPushDescriptors::AddBuffer(ResourceID bufferID, uint32_t bindingIndex)
	{
		auto buffer = ResourceManager::GetResource<VulkanBuffer>(bufferID);
		m_WriteDescriptors.push_back(buffer->GetDescriptorInfo());
		m_WriteDescriptors[m_WriteDescriptors.size() - 1].dstBinding = bindingIndex;
	}

	void VulkanPushDescriptors::AddTexture(ResourceID textureID, uint32_t bindingIndex)
	{
		auto texture = ResourceManager::GetResource<VulkanTexture>(textureID);
		m_WriteDescriptors.push_back(texture->GetDescriptorInfo());
		m_WriteDescriptors[m_WriteDescriptors.size() - 1].dstBinding = bindingIndex;
	}

	void VulkanPushDescriptors::AddRenderTexture(ResourceID textureID, uint32_t bindingIndex)
	{
		auto texture = ResourceManager::GetResource<VulkanRenderTexture>(textureID);
		m_WriteDescriptors.push_back(texture->GetDescriptorInfo());
		m_WriteDescriptors[m_WriteDescriptors.size() - 1].dstBinding = bindingIndex;
	}
	void VulkanPushDescriptors::Clear()
	{
		m_WriteDescriptors.clear();
	}
}