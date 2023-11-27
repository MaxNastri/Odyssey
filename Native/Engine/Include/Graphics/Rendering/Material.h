#pragma once
#include "Resource.h"
#include "ResourceHandle.h"

namespace Odyssey
{
	class VulkanShader;
	class VulkanTexture;

	class Material : public Resource
	{
	public:
		Material(ResourceHandle<VulkanShader> vertexShader, ResourceHandle<VulkanShader> fragmentShader);

	public:
		ResourceHandle<VulkanShader> GetVertexShader() { return m_VertexShader; }
		ResourceHandle<VulkanShader> GetFragmentShader() { return m_FragmentShader; }

	private:
		ResourceHandle<VulkanShader> m_VertexShader;
		ResourceHandle<VulkanShader> m_FragmentShader;
	};
}