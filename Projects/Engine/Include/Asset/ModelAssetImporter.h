#pragma once
#include "Vertex.h"
#include "Bone.h"
#include "BoneKeyframe.h"

namespace Odyssey
{
	struct MeshImportData
	{
		size_t ObjectCount;
		std::vector<size_t> HashIDs;
		std::vector<std::string> Names;
		std::vector<glm::mat4> WorldMatrices;
		std::vector<std::vector<Vertex>> VertexLists;
		std::vector<std::vector<uint32_t>> IndexLists;
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

	class ModelAssetImporter
	{
	public:
		virtual bool Import(const Path& modelPath) = 0;

	public:
		const MeshImportData& GetMeshData() { return m_MeshData; }
		const RigImportData& GetRigData() { return m_RigData; }
		const AnimationImportData& GetAnimationData(size_t index = 0) { return m_AnimationData[index]; }
		size_t GetClipCount() { return m_AnimationData.size(); }

	protected:
		MeshImportData m_MeshData;
		RigImportData m_RigData;
		std::vector<AnimationImportData> m_AnimationData;
	};
}