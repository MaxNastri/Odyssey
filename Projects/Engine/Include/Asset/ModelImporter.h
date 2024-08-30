#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include "Vertex.h"
#include "AnimationRig.h"

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
		std::map<std::string, Bone> m_BoneMap;
	};

	class ModelImporter
	{
	private:
		glm::mat4x4 AssimpToGLM(aiMatrix4x4 mat)
		{
			return glm::mat4x4(
				(float)mat.a1, (float)mat.b1, (float)mat.c1, (float)mat.d1,
				(float)mat.a2, (float)mat.b2, (float)mat.c2, (float)mat.d2,
				(float)mat.a3, (float)mat.b3, (float)mat.c3, (float)mat.d3,
				(float)mat.a4, (float)mat.b4, (float)mat.c4, (float)mat.d4
			);
		}
	public:
		void Import(const Path& modelPath)
		{
			Assimp::Importer importer;

			const aiScene* scene = importer.ReadFile(modelPath.string(), 0);

			if (scene->HasMeshes())
			{
				m_MeshDatas.clear();

				// TODO: Add support for submeshes
				//for (uint32_t m = 0; m < scene->mNumMeshes; m++)
				{
					// TODO: Add support for submeshes
					aiMesh* mesh = scene->mMeshes[0];
					MeshImportData importData;

					// Add to the vertex count
					importData.VertexCount += mesh->mNumVertices;

					// Add to the normals count
					if (mesh->HasNormals())
						importData.NormalsCount += mesh->mNumVertices;

					// Track the uv channel count
					importData.UVChannelCount = mesh->GetNumUVChannels();

					for (uint32_t i = 0; i < mesh->mNumVertices; i++)
					{
						Vertex vertex;
						vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

						if (mesh->HasNormals())
							vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

						// TODO: Add support for 8 texcoord channels
						if (mesh->HasTextureCoords(0))
						{
							vertex.TexCoord0 = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
							importData.UVChannels.push_back(0);
						}

						// TODO: Add support for up to 8 colors?
						if (mesh->HasVertexColors(0))
						{
							auto color = mesh->mColors[0][i];
							vertex.Color = glm::vec4(color.r, color.g, color.b, color.a);
						}

						importData.Vertices.push_back(vertex);
					}

					for (uint32_t i = 0; i < mesh->mNumFaces; i++)
					{
						aiFace face = mesh->mFaces[i];

						// Add to the index count
						importData.IndexCount += face.mNumIndices;

						for (uint32_t index = 0; index < face.mNumIndices; index++)
						{
							importData.Indices.push_back(face.mIndices[index]);
						}
					}

					// Animation
					if (mesh->HasBones())
					{
						uint32_t boneCount = 0;

						for (uint32_t i = 0; i < mesh->mNumBones; i++)
						{
							auto sourceBone = mesh->mBones[i];
							uint32_t boneIndex = 0;
							std::string boneName = sourceBone->mName.C_Str();

							if (m_RigData.m_BoneMap.contains(boneName))
							{
								boneIndex = m_RigData.m_BoneMap[boneName].Index;
							}
							else
							{
								boneIndex = boneCount;
								++boneCount;
							}

							auto& boneData = m_RigData.m_BoneMap[boneName];
							boneData.Name = boneName;
							boneData.Index = boneIndex;
							boneData.InverseBindpose = AssimpToGLM(sourceBone->mOffsetMatrix);

							for (uint32_t j = 0; j < sourceBone->mNumWeights; j++)
							{
								uint32_t vertexID = sourceBone->mWeights[j].mVertexId;
								float weight = sourceBone->mWeights[j].mWeight;
								if (weight > 0.0f)
									importData.SetBoneInfluence(vertexID, boneData.Index, weight);
							}
						}
					}

					m_MeshDatas.push_back(importData);
				}
			}
		}

		MeshImportData GetMeshData(uint32_t index)
		{
			return m_MeshDatas[index];
		}

		size_t GetMeshCount()
		{
			return m_MeshDatas.size();
		}

	private:
		std::vector<MeshImportData> m_MeshDatas;
		RigImportData m_RigData;
	};
}