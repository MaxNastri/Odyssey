#pragma once
#include "ModelImportData.hpp"

namespace tinygltf
{
	class Node;
	class Model;
}

namespace Odyssey
{
	using namespace tinygltf;

	class GLTFAssetImporter
	{
	public:
		bool Import(const Path& filePath);

	public:
		const MeshImportData& GetMeshData() { return m_MeshData; }
		const RigImportData& GetRigData() { return m_RigData; }
		const AnimationImportData& GetAnimationData() { return m_AnimationData; }

	private:
		void LoadMeshData(const Model* model);

	private:
		bool m_LoggingEnabled = false;

	private:
		MeshImportData m_MeshData;
		RigImportData m_RigData;
		AnimationImportData m_AnimationData;
	};
}