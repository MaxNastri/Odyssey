#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include "Vertex.h"
#include "Bone.h"

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

	class ModelImporter
	{
	public:
		void Import(const Path& modelPath);		
		MeshImportData& GetMeshData(uint32_t index) { return m_MeshDatas[index]; }
		RigImportData& GetRigData() { return m_RigData; }
		size_t GetMeshCount() { return m_MeshDatas.size(); }

	private:
		std::vector<MeshImportData> m_MeshDatas;
		RigImportData m_RigData;
	};
}