#pragma once

namespace Odyssey
{
	struct Bone
	{
	public:
		std::string Name;
		uint32_t Index;
		int32_t ParentIndex;
		glm::mat4 InverseBindpose;
		glm::mat4 Transform;
	};
}