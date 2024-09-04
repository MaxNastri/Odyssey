#pragma once
#include "ModelAssetImporter.hpp"

namespace fbxsdk
{
	class FbxIOSettings;
	class FbxManager;
	class FbxMesh;
	class FbxNode;
	class FbxScene;
}

using namespace fbxsdk;

namespace Odyssey
{

	class FBXModelImporter : public ModelAssetImporter
	{
	public:
		struct Settings
		{
		public:
			bool ConvertToLH = true;
			bool LoggingEnabled = false;
			float Scale = 0.01f;
			bool BakeGlobalTransform = false;
		};
	public:
		FBXModelImporter();
		FBXModelImporter(const Settings& settings);

	public:
		virtual bool Import(const Path& modelPath) override;

	private:
		void Init();
		bool ValidateFile(const Path& filePath);
		void LoadMeshNodeData(FbxNode* node);
		void LoadMeshData(FbxNode* meshNode);

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
	};
}