#pragma once
#include "ModelImportData.hpp"

namespace fbxsdk
{
	class FbxIOSettings;
	class FbxManager;
	class FbxMesh;
	class FbxNode;
	class FbxScene;
}

namespace Odyssey
{
	using namespace fbxsdk;

	class FBXModelImporter
	{
	public:
		struct Settings
		{
		public:
			bool ConvertToLH = true;
			bool LoggingEnabled = false;
		};
	public:
		FBXModelImporter();
		FBXModelImporter(const Settings& settings);

	public:
		void Import(const Path& filePath);

	public:
		const MeshImportData& GetMeshData() { return m_MeshData; }
		const RigImportData& GetRigData() { return m_RigData; }
		const AnimationImportData& GetAnimationData() { return m_AnimationData; }

	private:
		void Init();
		bool ValidateFile(const Path& filePath);
		void LoadMeshNodeData(FbxNode* node);
		void LoadMeshData(FbxNode* meshNode);
		void LoadAnimationData();

		void ProcessBoneHierarchy(FbxNode* node, int32_t boneIndex, int32_t parentIndex);

	private:
		glm::vec3 GetNormal(FbxMesh* mesh, int32_t vertexID, int32_t controlPoint);
		glm::vec3 GetTangent(FbxMesh* mesh, int32_t vertexID, int32_t controlPoint);
		glm::vec2 GetTexcoord(FbxMesh* mesh, int32_t vertexID, int32_t controlPoint, int32_t polygonPos);
		void GetBoneInfluences(FbxMesh* mesh);

	private:
		FbxManager* m_SDKManager;
		FbxIOSettings* m_FBXSettings;
		FbxScene* m_CurrentScene;
		Settings m_Settings;

	private:
		MeshImportData m_MeshData;
		RigImportData m_RigData;
		AnimationImportData m_AnimationData;
	};
}