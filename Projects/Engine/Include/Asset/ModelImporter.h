#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include "Vertex.h"

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
		std::vector<Vertex> Vertices;
		std::vector<uint32_t> Indices;
	};

	class ModelImporter
	{
	public:
		void Import(const Path& modelPath)
		{
			Assimp::Importer importer;

			const aiScene* scene = importer.ReadFile(modelPath.string(), 0);

			if (scene->HasMeshes())
			{
				m_MeshDatas.clear();

				for (uint32_t m = 0; m < scene->mNumMeshes; m++)
				{
					aiMesh* mesh = scene->mMeshes[m];
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
	};
}