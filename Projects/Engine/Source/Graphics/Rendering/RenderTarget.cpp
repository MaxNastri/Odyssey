#include "RenderTarget.h"
#include "ResourceManager.h"
#include "VulkanTexture.h"
#include "VulkanContext.h"

namespace Odyssey
{
	RenderTarget::RenderTarget(ResourceID id, std::shared_ptr<VulkanContext> context, VulkanImageDescription& imageDesc, RenderTargetFlags flags)
		: Resource(id)
	{
		if (flags & RenderTargetFlags::Color)
		{
			if (context->GetSampleCount() > 1)
				imageDesc.Samples = context->GetSampleCount();

			// Allocate the color texture
			m_ColorTexture = ResourceManager::Allocate<VulkanTexture>(imageDesc, nullptr);

			// Check if the texture is multisampled
			if (imageDesc.Samples > 1)
			{
				// Create a resolve texture
				VulkanImageDescription resolveDesc = imageDesc;
				resolveDesc.Samples = 1;
				m_ColorResolveTexture = ResourceManager::Allocate<VulkanTexture>(resolveDesc, nullptr);
			}
		}

		if (flags & RenderTargetFlags::Depth)
		{
			if (imageDesc.ImageType != ImageType::DepthTexture && imageDesc.ImageType != ImageType::Shadowmap)
				imageDesc.ImageType = ImageType::DepthTexture;

			if (imageDesc.ImageType != ImageType::Shadowmap && context->GetSampleCount() > 1)
				imageDesc.Samples = context->GetSampleCount();

			if (!IsDepthFormat(imageDesc.Format))
				imageDesc.Format = imageDesc.ImageType == ImageType::Shadowmap ?
				TextureFormat::D32_SFLOAT : TextureFormat::D24_UNORM_S8_UINT;

			m_DepthTexture = ResourceManager::Allocate<VulkanTexture>(imageDesc, nullptr);

			// Check if the texture is multisampled
			if (imageDesc.Samples > 1)
			{
				// Create a resolve texture
				VulkanImageDescription resolveDesc = imageDesc;
				resolveDesc.Samples = 1;
				m_DepthResolveTexture = ResourceManager::Allocate<VulkanTexture>(resolveDesc, nullptr);
			}
		}
	}

	RenderTarget::RenderTarget(ResourceID id, std::shared_ptr<VulkanContext> context, ResourceID imageID, TextureFormat format, RenderTargetFlags flags)
	{
		if (flags & RenderTargetFlags::Color)
		{
			if (context->GetSampleCount() > 1)
			{
				Ref<VulkanImage> image = ResourceManager::GetResource<VulkanImage>(imageID);
				VulkanImageDescription desc;
				desc.ImageType = ImageType::RenderTexture;
				desc.Width = image->GetWidth();
				desc.Height = image->GetHeight();
				desc.Samples = context->GetSampleCount();
				desc.Format = format;
				m_ColorTexture = ResourceManager::Allocate<VulkanTexture>(desc, nullptr);
				m_ColorResolveTexture = ResourceManager::Allocate<VulkanTexture>(imageID, format);
			}
			else
			{
				m_ColorTexture = ResourceManager::Allocate<VulkanTexture>(imageID, format);
			}
		}
		else if (flags & RenderTargetFlags::Depth)
		{
			m_DepthTexture = ResourceManager::Allocate<VulkanTexture>(imageID, format);
		}
	}
	void RenderTarget::Destroy()
	{
			ResourceManager::Destroy(m_ColorTexture);
			ResourceManager::Destroy(m_ColorResolveTexture);
			ResourceManager::Destroy(m_DepthTexture);
			ResourceManager::Destroy(m_DepthResolveTexture);
	}
}