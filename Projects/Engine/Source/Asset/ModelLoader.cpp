#include "ModelLoader.h"
#include "Material.h"
#include "Mesh.h"
#include "AssetManager.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Odyssey
{
	bool ModelLoader::LoadModel(const std::filesystem::path& assetPath, ModelAsset& outModel)
	{
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(assetPath.string(), aiProcess_FlipWindingOrder);

		if (!scene)
			return false;

		ProcessNode(scene->mRootNode, scene);

		outModel.Mesh = m_Meshes[0];
		return true;
	}

	void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene)
	{
		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			m_Meshes.push_back(ProcessMesh(mesh, scene));
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene);
		}
	}

	AssetHandle<Mesh> ModelLoader::ProcessMesh(aiMesh* importMesh, const aiScene* scene)
	{
		std::vector<VulkanVertex> vertices;
		std::vector<uint32_t> indices;

		vertices.resize(importMesh->mNumVertices);
		for (uint32_t i = 0; i < importMesh->mNumVertices; i++)
		{
			aiVector3D modelVertex = importMesh->mVertices[i];

			VulkanVertex vertex;
			vertex.Position = glm::vec3(modelVertex.x, modelVertex.y, modelVertex.z);

			if (importMesh->mTextureCoords[0])
			{
				aiVector3D uv0 = importMesh->mTextureCoords[0][i];
				vertex.TexCoord0 = glm::vec2(uv0.x, uv0.y);
			}

			if (importMesh->mNormals)
			{
				aiVector3D normals = importMesh->mNormals[i];
				vertex.Normal = glm::vec3(normals.x, normals.y, normals.z);
			}

			vertices[i] = vertex;
		}

		for (uint32_t i = 0; i < importMesh->mNumFaces; i++)
		{
			aiFace face = importMesh->mFaces[i];

			for (uint32_t j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		AssetHandle<Mesh> meshHandle = AssetManager::CreateMesh();
		if (Mesh* mesh = meshHandle.Get())
		{
			mesh->SetVertices(vertices);
			mesh->SetIndices(indices);
		}

		return meshHandle;
	}
}