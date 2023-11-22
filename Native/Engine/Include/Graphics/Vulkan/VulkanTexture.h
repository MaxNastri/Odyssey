#pragma once
#include "VulkanGlobals.h"
#include "VulkanImage.h"
#include "VulkanTextureSampler.h"

namespace Odyssey
{
	class VulkanContext;

	class VulkanTexture
	{
	public:
		VulkanTexture(std::shared_ptr<VulkanContext> context, const std::string& filename);
		VulkanTexture(std::shared_ptr<VulkanContext> context, uint32_t width, uint32_t height);
		void Destroy();

	public:
		VulkanImage* GetImage() { return m_Image.get(); }
		VulkanTextureSampler* GetSampler() { return m_Sampler.get(); }

	private:
		std::shared_ptr<VulkanContext> m_Context;
		std::unique_ptr<VulkanImage> m_Image;
		std::unique_ptr<VulkanTextureSampler> m_Sampler;
		// Sampler, view, layout
	};
}