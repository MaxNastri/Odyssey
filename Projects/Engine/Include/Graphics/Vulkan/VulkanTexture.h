#pragma once
#include "VulkanGlobals.h"
#include "Resource.h"
#include "VulkanImage.h"
#include "BinaryBuffer.h"

namespace Odyssey
{
	class VulkanContext;
	class VulkanTextureSampler;

	class VulkanTexture : public Resource
	{
	public:
		VulkanTexture(ResourceID id);
		VulkanTexture(ResourceID id, std::shared_ptr<VulkanContext> context, VulkanImageDescription description, BinaryBuffer* buffer);
		VulkanTexture(ResourceID id, std::shared_ptr<VulkanContext> context, ResourceID image, TextureFormat format);

	public:
		virtual void Destroy() override;

	public:
		uint32_t GetWidth() { return m_Description.Width; }
		uint32_t GetHeight() { return m_Description.Height; }
		ResourceID GetImage() { return m_Image; }
		ResourceID GetSampler() { return m_Sampler; }
		TextureFormat GetFormat() { return m_Description.Format; }
		VkWriteDescriptorSet GetDescriptorInfo();

	public:
		void CopyToTexture(ResourceID destination);

	public:
		void SetSampler(ResourceID samplerID) { m_Sampler = samplerID; }
		
	private:
		ResourceID m_Image;
		ResourceID m_Sampler;
		VkDescriptorImageInfo descriptor;
		VulkanImageDescription m_Description;
		std::shared_ptr<VulkanContext> m_Context;
	};
}