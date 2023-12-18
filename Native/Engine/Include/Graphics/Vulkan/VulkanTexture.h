#pragma once
#include "Resource.h"
#include "VulkanGlobals.h"
#include "VulkanImage.h"
#include "VulkanTextureSampler.h"
#include "ResourceHandle.h"

namespace Odyssey
{
	class VulkanContext;
	class VulkanBuffer;
	class VulkanTexture : public Resource
	{
	public:
		VulkanTexture(std::shared_ptr<VulkanContext> context, const std::string& filename);
		VulkanTexture(std::shared_ptr<VulkanContext> context, uint32_t width, uint32_t height);
		void Destroy();

	public:
		void SetData(const void* data);

	public:
		ResourceHandle<VulkanImage> GetImage() { return m_Image; }
		ResourceHandle<VulkanTextureSampler> GetSampler() { return m_Sampler; }
		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }

	private:
		std::shared_ptr<VulkanContext> m_Context;
		ResourceHandle<VulkanImage> m_Image;
		ResourceHandle<VulkanTextureSampler> m_Sampler;
		ResourceHandle<VulkanBuffer> m_StagingBuffer;
		uint32_t m_Width, m_Height;
		// Sampler, view, layout
	};
}