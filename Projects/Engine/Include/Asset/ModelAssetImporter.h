#pragma once
#include "Vertex.h"
#include "Bone.h"
#include "BoneKeyframe.h"

namespace Odyssey
{
	struct SubmeshImportData
	{
		uint32_t Index = 0;
		std::vector<Vertex> Vertices;
		std::vector<uint32_t> Indices;
	};

	struct MeshImportData
	{
		std::string Name;
		std::vector<SubmeshImportData> Submeshes;
	};

	struct FBXBone
	{
		std::string Name;
		int32_t ParentIndex = -1;
		int32_t Index = -1;
		glm::mat4 inverseBindpose;
		int32_t NodeIndex = -1;
	};

	struct BoneInfluence
	{
		glm::vec4 Indices;
		glm::vec4 Weights;
	};

	struct RigImportData
	{
		std::unordered_map<std::string, FBXBone> Bones;
		glm::mat4 RotationOffset = glm::mat4(1.0f);
		glm::mat4 ScaleOffset = glm::mat4(1.0f);
		size_t BoneCount = 0;
	};

	struct AnimationImportData
	{
	public:
		std::string Name;
		double Start = std::numeric_limits<double>::max();
		double Duration = 0.0;
		uint32_t FramesPerSecond = 0;
		std::map<std::string, BoneKeyframe> BoneKeyframes;
	};

	struct PrefabImportData
	{
	public:
		struct Node
		{
			std::string Name;
			mat4 Transform;
		};

		std::vector<Node> Nodes;
	};

	class ModelAssetImporter
	{
	public:
		virtual bool Import(const Path& modelPath) = 0;

	public:
		const MeshImportData& GetMeshData(size_t index = 0) { return m_MeshDatas[index]; }
		uint32_t MeshCount() { return (uint32_t)m_MeshDatas.size(); }

	public:
		const PrefabImportData& GetPrefabData() { return m_PrefabImportData; }
		const RigImportData& GetRigData() { return m_RigData; }
		const AnimationImportData& GetAnimationData(size_t index = 0) { return m_AnimationData[index]; }
		size_t GetClipCount() { return m_AnimationData.size(); }

	protected:
		std::vector<MeshImportData> m_MeshDatas;
		RigImportData m_RigData;
		std::vector<AnimationImportData> m_AnimationData;
		PrefabImportData m_PrefabImportData;
	};
}