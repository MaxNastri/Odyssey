#pragma once
#include "Asset.h"
#include "Bone.h"

namespace Odyssey
{
	class SourceModel;

	class AnimationRig : public Asset
	{
	public:
		AnimationRig(const Path& assetPath);
		AnimationRig(const Path& assetPath, std::shared_ptr<SourceModel> source);

	public:
		void Save();
		void Load();

	public:
		std::vector<glm::mat4> GetBindposes() { return m_Bindposes; }

	private:
		void SaveToDisk(const Path& assetPath);
		void LoadFromDisk(const Path& assetPath);

	private:
		std::vector<Bone> m_Bones;
		std::vector<glm::mat4> m_Bindposes;
	};
}