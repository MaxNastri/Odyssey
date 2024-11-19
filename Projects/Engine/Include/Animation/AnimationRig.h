#pragma once
#include "Asset.h"
#include "Bone.h"

namespace Odyssey
{
	class SourceModel;

	class AnimationRig : public Asset
	{
		CLASS_DECLARATION(Odyssey, AnimationRig)
	public:
		AnimationRig(const Path& assetPath);
		AnimationRig(const Path& assetPath, std::shared_ptr<SourceModel> source);

	public:
		void Save();
		void Load();

	public:
		const std::vector<Bone>& GetBones() { return m_Bones; }
		const glm::mat4& GetGlobalMatrix() { return m_GlobalMatrix; }
		const glm::mat4 GetRotationOffset() { return m_RotationOffset; }

		const Bone& GetRootBone() { return m_Bones[m_RootBone]; }

	private:
		void LoadFromSource(std::shared_ptr<SourceModel> source);
		void SaveToDisk(const Path& assetPath);

	private:
		void OnSourceModified();

	private:
		std::uint32_t m_RootBone;
		std::vector<Bone> m_Bones;
		glm::mat4 m_GlobalMatrix;
		glm::mat4 m_RotationOffset;
		glm::mat4 m_ScaleOffset;
	};
}