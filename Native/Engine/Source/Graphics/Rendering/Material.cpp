#include "Material.h"

namespace Odyssey
{
	Material::Material(ResourceHandle<VulkanShader> vertexShader, ResourceHandle<VulkanShader> fragmentShader)
	{
		m_VertexShader = vertexShader;
		m_FragmentShader = fragmentShader;
	}
}