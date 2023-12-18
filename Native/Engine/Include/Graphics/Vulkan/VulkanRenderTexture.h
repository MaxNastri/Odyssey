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
	class VulkanRenderTexture : public Resource
	{
	public:
		VulkanRenderTexture(std::shared_ptr<VulkanContext> context, uint32_t width, uint32_t height);
		VulkanRenderTexture(std::shared_ptr<VulkanContext> context, uint32_t width, uint32_t height, TextureFormat format);
		VulkanRenderTexture(std::shared_ptr<VulkanContext> context, ResourceHandle<VulkanImage> image, TextureFormat format);
		void Destroy();

	public:
		void SetData(const void* data);

	public:
		ResourceHandle<VulkanImage> GetImage() { return m_Image; }
		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }

	private:
		bool IsDepthTexture(TextureFormat format);

	private:
		std::shared_ptr<VulkanContext> m_Context;
		ResourceHandle<VulkanImage> m_Image;
		ResourceHandle<VulkanBuffer> m_StagingBuffer;
		uint32_t m_Width, m_Height;
	};
}