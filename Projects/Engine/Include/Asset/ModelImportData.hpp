#pragma once
#include "Vertex.h"
#include "Bone.h"
#include "BoneKeyframe.hpp"

namespace Odyssey
{
	struct MeshImportData
	{
	public: // Inspector data
		uint32_t VertexCount = 0;
		uint32_t IndexCount = 0;
		uint32_t UVChannelCount = 0;
		uint32_t TangentsCount = 0;
		uint32_t NormalsCount = 0;
		std::vector<uint32_t> UVChannels;

	public:
		void Clear()
		{
			Vertices.clear();
			Indices.clear();
			m_VertexBoneTracking.clear();
		}

		void SetBoneInfluence(uint32_t vertexID, uint32_t boneIndex, float boneWeight)
		{
			// Insert 0 as a default
			if (!m_VertexBoneTracking.contains(vertexID))
				m_VertexBoneTracking[vertexID] = 0;

			// Use the current influence count as the index into the vec4 for indices/weights
			uint32_t element = m_VertexBoneTracking[vertexID];
			if (element < 4)
			{
				Vertices[vertexID].BoneIndices[element] = (float)boneIndex;
				Vertices[vertexID].BoneWeights[element] = boneWeight;

				if (glm::length(Vertices[vertexID].BoneWeights) > 1.0f)
					int dbeug = 0;
			}

			// Increment the influence count so we move to the next vec4 element
			m_VertexBoneTracking[vertexID]++;
		}

	public:
		std::vector<Vertex> Vertices;
		std::vector<uint32_t> Indices;

	private:
		// <VertexID, InfluenceCount>
		std::map<uint32_t, uint32_t> m_VertexBoneTracking;
	};

	struct RigImportData
	{
	public:
		glm::mat4 GlobalTransform;
		std::map<std::string, Bone> m_BoneMap;
	};

	struct BoneKeyframeData
	{
	public:
		template<typename T> struct Key { double Time; T Value; };
		typedef Key<glm::mat4> PositionKey;
		typedef Key<glm::mat4> RotationKey;
		typedef Key<glm::mat4> ScaleKey;


		std::vector<PositionKey> PositionKeys;
		std::vector<RotationKey> RotationKeys;
		std::vector<ScaleKey> ScaleKeys;
	};

	struct AnimationImportData
	{
	public:
		std::string Name;
		double Duration;
		std::map<std::string, BoneKeyframe> BoneKeyframes;
	};
}