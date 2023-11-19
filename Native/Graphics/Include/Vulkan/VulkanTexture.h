#pragma once
#include "VulkanGlobals.h"

namespace Odyssey
{
	class VulkanContext;

	class VulkanTexture
	{
	public:
		VulkanTexture(std::shared_ptr<VulkanContext> context, const std::string& filename);

	private:
		std::shared_ptr<VulkanContext> m_Context;

	};
}