#pragma once
#include "Vertex.h"
#include "Bone.h"
#include "BoneKeyframe.hpp"

namespace fbxsdk
{
	class FbxNode;
}

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
		fbxsdk::FbxNode* Node;
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
		std::vector<BoneInfluence> ControlPointInfluences;
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
		double Duration;
		uint32_t FramesPerSecond;
		std::map<std::string, BoneKeyframe> BoneKeyframes;
	};

	class ModelAssetImporter
	{
	public:
		virtual bool Import(const Path& modelPath) = 0;

	public:
		const MeshImportData& GetMeshData() { return m_MeshData; }
		const RigImportData& GetRigData() { return m_RigData; }
		const AnimationImportData& GetAnimationData() { return m_AnimationData; }

	protected:
		MeshImportData m_MeshData;
		RigImportData m_RigData;
		AnimationImportData m_AnimationData;
	};
}