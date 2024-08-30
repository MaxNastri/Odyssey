#pragma once

namespace Odyssey
{
	struct Bone
	{
	public:
		std::string Name;
		glm::mat4 InverseBindpose;
		uint32_t Index;
	};
}