#pragma once
#include "Resource.h"
#include "VulkanImage.h"

namespace Odyssey
{
	enum RenderTargetFlags
	{
		Color = 1 << 0,
		Depth = 1 << 1,
	};

	DEFINE_ENUM_FLAG_OPERATORS(RenderTargetFlags);

	class RenderTarget : public Resource
	{
	public:
		RenderTarget(ResourceID id, std::shared_ptr<VulkanContext> context, VulkanImageDescription& imageDesc, RenderTargetFlags flags);
		RenderTarget(ResourceID id, std::shared_ptr<VulkanContext> context, ResourceID image, TextureFormat format, RenderTargetFlags flags);

	public:
		virtual void Destroy() override;

	public:
		ResourceID GetColorTexture() { return m_ColorTexture; }
		ResourceID GetColorResolveTexture() { return m_ColorResolveTexture; }
		ResourceID GetDepthTexture() { return m_DepthTexture; }
		ResourceID GetDepthResolveTexture() { return m_DepthResolveTexture; }

	private:
		ResourceID m_ColorTexture;
		ResourceID m_ColorResolveTexture;
		ResourceID m_DepthTexture;
		ResourceID m_DepthResolveTexture;
	};
}