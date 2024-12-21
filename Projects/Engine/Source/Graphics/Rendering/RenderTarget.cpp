#include "RenderTarget.h"
#include "ResourceManager.h"
#include "VulkanTexture.h"

namespace Odyssey
{
	RenderTarget::RenderTarget(ResourceID id, std::shared_ptr<VulkanContext> context, VulkanImageDescription& imageDesc, RenderTargetFlags flags)
		: Resource(id)
	{
		if (flags & RenderTargetFlags::Color)
		{
			// Allocate the color texture
			m_ColorTexture = ResourceManager::Allocate<VulkanTexture>(imageDesc, nullptr);

			// Check if the texture is multisampled
			if (imageDesc.Samples > 1)
			{
				// Create a resolve texture
				VulkanImageDescription resolveDesc = imageDesc;
				resolveDesc.Samples = 1;
				m_ColorResolveTexture = ResourceManager::Allocate<VulkanImage>(resolveDesc);
			}
		}

		if (flags & RenderTargetFlags::Depth)
		{
			if (!IsDepthFormat(imageDesc.Format))
				imageDesc.Format = TextureFormat::D24_UNORM_S8_UINT;

			m_DepthTexture = ResourceManager::Allocate<VulkanTexture>(imageDesc, nullptr);

			// Check if the texture is multisampled
			if (imageDesc.Samples > 1)
			{
				// Create a resolve texture
				VulkanImageDescription resolveDesc = imageDesc;
				resolveDesc.Samples = 1;
				m_ColorResolveTexture = ResourceManager::Allocate<VulkanImage>(resolveDesc);
			}
		}
	}

	RenderTarget::RenderTarget(ResourceID id, std::shared_ptr<VulkanContext> context, ResourceID image, TextureFormat format, RenderTargetFlags flags)
	{
		if (flags & RenderTargetFlags::Color)
			m_ColorTexture = ResourceManager::Allocate<VulkanTexture>(image, format);
		else if (flags & RenderTargetFlags::Depth)
			m_DepthTexture = ResourceManager::Allocate<VulkanTexture>(image, format);
	}
}