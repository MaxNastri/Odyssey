#pragma once
#include "ModelImportData.hpp"

struct aiAnimation;
struct aiMesh;
struct aiScene;

namespace Odyssey
{

	class ModelImporter
	{
	public:
		void Import(const Path& modelPath);		
		MeshImportData& GetMeshData(uint32_t index) { return m_MeshData; }
		RigImportData& GetRigData() { return m_RigData; }
		AnimationImportData& GetAnimationData() { return m_AnimationData; }
		size_t GetMeshCount() { return m_MeshCount; }

	private:
		void NormalizeMeshWeights(const aiMesh* mesh);
		void ImportMesh(const aiMesh* mesh);
		void ImportRig(const aiScene* scene);
		void ImportAnimationClip(const aiAnimation* animation);

	private:
		uint32_t m_MeshCount;
		MeshImportData m_MeshData;
		RigImportData m_RigData;
		AnimationImportData m_AnimationData;
	};
}