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
		int32_t ParentIndex;
		int32_t Index;
		glm::mat4 bindpose;
		glm::mat4 inverseBindpose;
	};

	struct BoneInfluence
	{
		glm::vec4 Indices;
		glm::vec4 Weights;
	};

	struct RigImportData
	{
		std::vector<FBXBone> FBXBones;
		std::vector<BoneInfluence> ControlPointInfluences;
	};

	struct AnimationImportData
	{
	public:
		std::string Name;
		double Duration;
		uint32_t FramesPerSecond;
		std::map<std::string, BoneKeyframe> BoneKeyframes;
	};
}