#pragma once

namespace Odyssey
{
	class VulkanShader;

	struct VulkanPipelineInfo
	{
		VulkanShader* fragmentShader;
		VulkanShader* vertexShader;
	};
}