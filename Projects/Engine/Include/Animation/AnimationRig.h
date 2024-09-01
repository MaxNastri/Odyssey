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
		const std::vector<Bone>& GetBones() { return m_Bones; }
		const glm::mat4& GetTransform() { return m_Transform; }

	private:
		void SaveToDisk(const Path& assetPath);
		void LoadFromDisk(const Path& assetPath);

	private:
		glm::mat4 m_Transform;
		std::vector<Bone> m_Bones;
	};
}