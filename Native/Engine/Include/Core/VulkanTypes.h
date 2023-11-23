#pragma once
#include "ResourceHandle.h"

namespace Odyssey
{
	class VulkanShader;

	struct VulkanPipelineInfo
	{
	public:
		ResourceHandle<VulkanShader> fragmentShader;
		ResourceHandle<VulkanShader> vertexShader;
	};
}