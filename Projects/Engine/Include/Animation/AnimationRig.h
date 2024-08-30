#pragma once
#include "Asset.h"

namespace Odyssey
{
	struct Bone
	{
	public:
		std::string Name;
		glm::mat4x4 InverseBindpose;
		uint32_t Index;
	};

	class AnimationRig : public Asset
	{
	public:
		AnimationRig(const Path& assetPath);

	public:
		void Save();
		void Load();

	private:
		void SaveToDisk(const Path& assetPath);
		void LoadFromDisk(const Path& assetPath);

	private:
		std::vector<Bone> m_Bones;
		std::vector<glm::mat4x4> m_Bindposes;
	};
}