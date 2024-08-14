#pragma once
#include "glm.h"
#include "VulkanVertex.h"

namespace Odyssey
{
	class GeometryUtil
	{
	public:
		static void setHandedness(bool rightHanded);
		static void ComputeBox(vec3 center, vec3 scale, std::vector<VulkanVertex>& vertices, std::vector<uint32_t>& indices);
		static void ComputeSphere(float radius, uint32_t tesselation, std::vector<VulkanVertex>& vertices, std::vector<uint32_t>& indices);
		static void ComputeDiamond(std::vector<VulkanVertex>& vertices, std::vector<uint32_t>& indices);
	private:
		static void createLine();
		static void createRing();
		static void ReverseWinding(std::vector<VulkanVertex>& vertices, std::vector<uint32_t>& indices);
	};
}