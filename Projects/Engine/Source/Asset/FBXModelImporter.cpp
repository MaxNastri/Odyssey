#include "FBXModelImporter.h"
#include "fbxsdk.h"
#include "Logger.h"

namespace Odyssey
{
	namespace Utils
	{
		struct VertexHash
		{
			std::size_t operator()(const Vertex& v) const
			{
				return size_t(std::hash<float>()(v.Position.x) +
					std::hash<float>()(v.Position.y) +
					std::hash<float>()(v.Position.z) +
					std::hash<float>()(v.Normal.x) +
					std::hash<float>()(v.Normal.y) +
					std::hash<float>()(v.Normal.z) +
					std::hash<float>()(v.Tangent.x) +
					std::hash<float>()(v.Tangent.y) +
					std::hash<float>()(v.Tangent.z) +
					std::hash<float>()(v.TexCoord0.x) +
					std::hash<float>()(v.TexCoord0.y) +
					std::hash<float>()(v.BoneWeights.x) +
					std::hash<float>()(v.BoneWeights.y) +
					std::hash<float>()(v.BoneWeights.z) +
					std::hash<float>()(v.BoneWeights.w) +
					std::hash<float>()(v.BoneIndices.x) +
					std::hash<float>()(v.BoneIndices.y) +
					std::hash<float>()(v.BoneIndices.z) +
					std::hash<float>()(v.BoneIndices.w));
			}
		};

		struct VertexEqual
		{
			float epsilon = 0.0f;
			bool operator()(const Vertex& v1, const Vertex& v2) const
			{
				return (v1.Position.x == v2.Position.x) &&
					(v1.Position.y == v2.Position.y) &&
					(v1.Position.z == v2.Position.z) &&
					(v1.Normal.x == v2.Normal.x) &&
					(v1.Normal.y == v2.Normal.y) &&
					(v1.Normal.z == v2.Normal.z) &&
					(v1.Tangent.x == v2.Tangent.x) &&
					(v1.Tangent.y == v2.Tangent.y) &&
					(v1.Tangent.z == v2.Tangent.z) &&
					(v1.TexCoord0.x == v2.TexCoord0.x) &&
					(v1.TexCoord0.y == v2.TexCoord0.y) &&
					(v1.BoneWeights.x == v2.BoneWeights.x) &&
					(v1.BoneWeights.y == v2.BoneWeights.y) &&
					(v1.BoneWeights.z == v2.BoneWeights.z) &&
					(v1.BoneWeights.z == v2.BoneWeights.z) &&
					(v1.BoneIndices.x == v2.BoneIndices.x) &&
					(v1.BoneIndices.y == v2.BoneIndices.y) &&
					(v1.BoneIndices.z == v2.BoneIndices.z) &&
					(v1.BoneIndices.w == v2.BoneIndices.w);
			}
		};


		inline static glm::mat4 ToGLM(FbxAMatrix fbxM)
		{
			return glm::mat4(
				(float)(fbxM.Get(0, 0)), (float)(fbxM.Get(0, 1)), (float)(fbxM.Get(0, 2)), (float)(fbxM.Get(0, 3)),
				(float)(fbxM.Get(1, 0)), (float)(fbxM.Get(1, 1)), (float)(fbxM.Get(1, 2)), (float)(fbxM.Get(1, 3)),
				(float)(fbxM.Get(2, 0)), (float)(fbxM.Get(2, 1)), (float)(fbxM.Get(2, 2)), (float)(fbxM.Get(2, 3)),
				(float)(fbxM.Get(3, 0)), (float)(fbxM.Get(3, 1)), (float)(fbxM.Get(3, 2)), (float)(fbxM.Get(3, 3))
			);
		}

		inline static glm::mat4 ConvertLH(FbxAMatrix fbxM)
		{
			// Get the translation and rotation of the world matrix
			FbxVector4 translation = fbxM.GetT();
			FbxVector4 rotation = fbxM.GetR();
			// Invert the z position and x/y rotation
			translation.Set(translation.mData[0], translation.mData[1], -translation.mData[2]);
			rotation.Set(-rotation.mData[0], -rotation.mData[1], rotation.mData[2]);

			// Construct a new world matrix from the inverted translation/rotation
			FbxAMatrix returnMatrix = fbxM;
			returnMatrix.SetT(translation);
			returnMatrix.SetR(rotation);

			return ToGLM(returnMatrix);
		}

		inline static glm::mat4 ConvertLHSwapYZ(FbxAMatrix fbxM, bool rotate)
		{
			glm::mat4 mat = ToGLM(fbxM);
			mat = glm::mat4(
				mat[0][0], mat[0][2], mat[0][1], mat[0][3],
				mat[2][0], mat[2][2], mat[2][1], mat[2][3],
				mat[1][0], mat[1][2], mat[1][1], mat[1][3],
				mat[3][0], mat[3][2], mat[3][1], mat[3][3]
			);
			return mat;
		}

		inline static glm::vec2 ToGLM2(FbxVector2 vec)
		{
			return glm::vec2(vec[0], vec[1]);
		}

		inline static glm::vec3 ToGLM3(FbxVector4 vec)
		{
			return glm::vec3(vec[0], vec[1], vec[2]);
		}

		inline static glm::vec4 ToGLM4(FbxVector4 vec)
		{
			return glm::vec4(vec[0], vec[1], vec[2], vec[3]);
		}


		inline static FbxAMatrix GetGeoTransformation(FbxNode* node)
		{
			assert(node);

			const FbxVector4 translation = node->GetGeometricTranslation(FbxNode::eSourcePivot);
			const FbxVector4 rotation = node->GetGeometricRotation(FbxNode::eSourcePivot);
			const FbxVector4 scale = node->GetGeometricScaling(FbxNode::eSourcePivot);
			return FbxAMatrix(translation, rotation, scale);
		}
	}

	FBXModelImporter::FBXModelImporter()
	{
		Init();
	}

	FBXModelImporter::FBXModelImporter(const Settings& settings)
		: m_Settings(settings)
	{
		Init();
	}

	void FBXModelImporter::Init()
	{
		m_SDKManager = FbxManager::Create();
		if (!m_SDKManager)
		{
			Logger::LogError("[FBXModelImporter] Failed to create SDK manager.");
			return;
		}

		m_FBXSettings = FbxIOSettings::Create(m_SDKManager, IOSROOT);
		m_SDKManager->SetIOSettings(m_FBXSettings);
		m_CurrentScene = FbxScene::Create(m_SDKManager, "Import Scene");

		if (!m_CurrentScene)
		{
			Logger::LogError("[FBXModelImporter] Failed to created scene.");
		}
	}

	bool FBXModelImporter::Import(const Path& modelPath)
	{
		assert(modelPath.extension() == ".fbx");

		if (!ValidateFile(modelPath))
			return false;

		// Triangulate the scene
		// TODO: Put this behind a config flag
		FbxGeometryConverter geoConverter(m_SDKManager);
		geoConverter.Triangulate(m_CurrentScene, false);

		// Process a bone hierarchy if it exists
		ProcessBoneHierarchy(m_CurrentScene->GetRootNode(), 0, -1);

		// Set the global matrix for the rig to a 180 rotation matrix if we are converting LH
		if (m_Settings.ConvertToLH)
		{
			glm::mat4 scale = glm::scale(glm::identity<mat4>(), glm::vec3(m_Settings.Scale, m_Settings.Scale, m_Settings.Scale));
			glm::mat4 rotation = glm::mat4_cast(glm::quat(glm::vec3(0, glm::radians(180.0f), 0)));
			m_RigData.GlobalMatrix = rotation * scale;
		}

		// Get the root node of the scene
		if (FbxNode* node = m_CurrentScene->GetRootNode())
		{
			// If a root node was found get the number of child nodes
			int nodeChildren = node->GetChildCount();

			// Iterate through each children to load the node's data
			for (int i = 0; i < nodeChildren; i++)
			{
				LoadMeshNodeData(node->GetChild(i));
			}
		}

		return true;
	}


	bool FBXModelImporter::ValidateFile(const Path& filePath)
	{
		int i, animationStackCount;
		bool status;

		// Get the file version number generated by the fbx sdk
		int32_t sdkMajor, sdkMinor, sdkRevision;
		FbxManager::GetFileFormatVersion(sdkMajor, sdkMinor, sdkRevision);

		// Create the importer
		FbxImporter* importer = FbxImporter::Create(m_SDKManager, "Importer");

		// Initialize the importer with a filename and get the file version
		const bool importStatus = importer->Initialize(filePath.string().c_str(), -1, m_FBXSettings);
		int32_t fileMajor, fileMinor, fileRevision;
		importer->GetFileVersion(fileMajor, fileMinor, fileRevision);

		// The import failed
		if (!importStatus)
		{
			// Report the error
			FbxString error = importer->GetStatus().GetErrorString();
			Logger::LogError("[FBXModelImporter] Failed to initialize importer.");
			Logger::LogError(error.Buffer());

			// Check if the file formats do not match up
			if (importer->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
			{
				Logger::LogError("[FBXModelImporter] Invalid file version detected.");
				Logger::LogError(std::format("FBX SDK version is {}.{}.{}", sdkMajor, sdkMinor, sdkRevision));
				Logger::LogError(std::format("File format is {}.{}.{}", fileMajor, fileMinor, fileRevision));
			}

			return false;
		}

		// Check if the file is an fbx file
		if (importer->IsFBX())
		{
			if (m_Settings.LoggingEnabled)
			{
				Logger::LogInfo("[FBXModelImporter] Animation Stack Information");
				animationStackCount = importer->GetAnimStackCount();

				Logger::LogInfo(std::format("Number of Animation Stacks: {}", animationStackCount));
				Logger::LogInfo(std::format("Current Animation Stack: {}", importer->GetActiveAnimStackName().Buffer()));

				for (i = 0; i < animationStackCount; i++)
				{
					FbxTakeInfo* lTakeInfo = importer->GetTakeInfo(i);

					Logger::LogInfo(std::format("Animation Stack {}", i));
					Logger::LogInfo(std::format("Name: {}", lTakeInfo->mName.Buffer()));
					Logger::LogInfo(std::format("Description: {}", lTakeInfo->mDescription.Buffer()));

					// Change the value of the import name if the animation stack should be imported 
					// under a different name.
					Logger::LogInfo(std::format("Import Name: {}", lTakeInfo->mImportName.Buffer()));

					// Set the value of the import state to false if the animation stack should be not
					// be imported. 
					Logger::LogInfo(std::format("Import State: {}", lTakeInfo->mSelect ? "true" : "false"));
				}
			}
		}

		// Import the scene.
		status = importer->Import((FbxDocument*)m_CurrentScene);

		// Check for a password failed import
		if (!status && importer->GetStatus().GetCode() == FbxStatus::ePasswordError)
		{
			Logger::LogError("[FBXModelImporter] Failed to import file: Password required.");
			return false;
		}

		// Destroy the importer.
		importer->Destroy();
		return true;
	}

	void FBXModelImporter::LoadMeshNodeData(FbxNode* node)
	{
		// Store the node's attribute type
		FbxNodeAttribute::EType attributeType = (node->GetNodeAttribute()->GetAttributeType());

		// Look for a mesh node
		switch (attributeType)
		{
			case FbxNodeAttribute::eMesh:
			{
				LoadMeshData(node);
				break;
			}
		}

		// Recurse through this node's children and perform the same process
		int32_t nodeChildren = node->GetChildCount();
		for (int32_t i = 0; i < nodeChildren; i++)
		{
			LoadMeshNodeData(node->GetChild(i));
		}
	}

	void FBXModelImporter::LoadMeshData(FbxNode* meshNode)
	{
		// Create the hash table to store unique vertices
		typedef std::unordered_map<Vertex, uint32_t, Utils::VertexHash, Utils::VertexEqual> VertexHashMap;

		VertexHashMap vertexHashMap;
		VertexHashMap::hasher hashFunc = vertexHashMap.hash_function();

		// Create the fbx mesh from the node
		FbxMesh* mesh = (FbxMesh*)meshNode->GetNodeAttribute();

		// Catalog the bone influences, if skinned
		GetBoneInfluences(mesh);

		// Get the vertices of the mesh
		FbxVector4* controlPoints = mesh->GetControlPoints();

		// Store a static counter for the vertex id of the current mesh
		int32_t vertexId = 0;

		std::vector<Vertex> vertexList;
		std::vector<uint32_t> indexList;
		size_t hashID = 0;

		// Get the number of polygons contained inside the mesh
		int32_t polygonCount = mesh->GetPolygonCount();

		// Iterate through each polygon of the mesh
		for (int32_t i = 0; i < polygonCount; i++)
		{
			// Get the number of vertices per polygon
			int polygonSize = mesh->GetPolygonSize(i);

			// Iterate through each vertex
			for (int j = 0; j < polygonSize; j++)
			{
				// Get the current index
				int controlPointIndex = mesh->GetPolygonVertex(i, j);

				// Create a new vertex
				Vertex vertex;

				// Get the position of the current vertex
				vertex.Position = Utils::ToGLM3(controlPoints[controlPointIndex]);
				vertex.Normal = GetNormal(mesh, vertexId, controlPointIndex);
				vertex.Tangent = GetTangent(mesh, vertexId, controlPointIndex);
				vertex.TexCoord0 = GetTexcoord(mesh, j, controlPointIndex, i);

				// IMPORTANT: Negate the z component and rotate 180 degrees to convert RH to LH
				if (m_Settings.ConvertToLH)
				{
					// NOTE: Swapping y and z is a separate conversion
					vertex.Position.z = -vertex.Position.z;
					vertex.TexCoord0.y = 1.0f - vertex.TexCoord0.y;
					vertex.Normal.z = -vertex.Normal.z;
					vertex.Tangent.z = -vertex.Tangent.z;

					// Non-skinned mesh
					if (m_Settings.BakeGlobalTransform)
						vertex.Position = m_RigData.GlobalMatrix * glm::vec4(vertex.Position, 1.0f);
				}

				// Check if this mesh is animated
				if (m_RigData.Bones.size() > 0)
				{
					// Transfer the indices and normalize the weights
					vertex.BoneIndices = m_RigData.ControlPointInfluences[controlPointIndex].Indices;
					vertex.BoneWeights = m_RigData.ControlPointInfluences[controlPointIndex].Weights;

					// Normalize the bone weights
					float totalWeights = vertex.BoneWeights.x + vertex.BoneWeights.y + vertex.BoneWeights.z + vertex.BoneWeights.w;
					vertex.BoneWeights = vertex.BoneWeights / totalWeights;
				}
				else
				{
					// Default to 0 values for the indices and weights
					// This is fine because a bone weight of 0 has no impact on the vertex's position regardless of what bone it references
					vertex.BoneIndices = glm::vec4(0, 0, 0, 0);
					vertex.BoneWeights = glm::vec4(0, 0, 0, 0);
				}

				// Determine if this vertex is unique or not
				if (!vertexHashMap.contains(vertex))
				{
					// This is a unique vertex
					uint32_t index = (uint32_t)(vertexList.size());
					vertexHashMap[vertex] = index;
					indexList.push_back(index);
					vertexList.push_back(vertex);
				}
				else
				{
					// This is not a unique vertex
					indexList.push_back(vertexHashMap[vertex]);
				}

				hashID += hashFunc(vertex);
				vertexId++;
			}
		}

		// Convert the world matrix to a directx type for storage
		glm::mat4 world = Utils::ToGLM(meshNode->EvaluateGlobalTransform());
		std::string name = meshNode->GetName();

		// IMPORTANT: Reverse the winding order to convert from RH to LH
		if (m_Settings.ConvertToLH)
			std::reverse(indexList.begin(), indexList.end());

		m_MeshData.HashIDs.push_back(hashID);
		m_MeshData.Names.push_back(name);
		m_MeshData.WorldMatrices.push_back(world);
		m_MeshData.VertexLists.push_back(vertexList);
		m_MeshData.IndexLists.push_back(indexList);

		// Load material info for this mesh
		//loadNodeMaterial(node);
	}

	void FBXModelImporter::ProcessBoneHierarchy(FbxNode* node, int32_t boneIndex, int32_t parentIndex)
	{
		if (node->GetNodeAttribute() && node->GetNodeAttribute()->GetAttributeType() && node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		{
			FBXBone bone;
			bone.Node = node;
			bone.Name = node->GetName();
			bone.Index = boneIndex;
			bone.ParentIndex = parentIndex;
			bone.inverseBindpose = glm::identity<glm::mat4>();
			m_RigData.Bones[bone.Name] = bone;
			parentIndex = boneIndex;
		}

		for (int i = 0; i < node->GetChildCount(); i++)
		{
			ProcessBoneHierarchy(node->GetChild(i), (int32_t)m_RigData.Bones.size(), parentIndex);
		}
	}

	glm::vec3 FBXModelImporter::GetNormal(FbxMesh* mesh, int32_t vertexID, int32_t controlPoint)
	{
		glm::vec3 normal(0, 0, 0);

		for (int l = 0; l < mesh->GetElementNormalCount(); ++l)
		{
			FbxGeometryElementNormal* vertexNormal = mesh->GetElementNormal(l);

			if (vertexNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				if (vertexNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					auto fbxNormal = vertexNormal->GetDirectArray().GetAt(controlPoint);
					normal = Utils::ToGLM3(fbxNormal);
				}
				else if (vertexNormal->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					int32_t id = vertexNormal->GetIndexArray().GetAt(controlPoint);
					auto fbxNormal = vertexNormal->GetDirectArray().GetAt(id);
					normal = Utils::ToGLM3(fbxNormal);
				}
			}

			if (vertexNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				if (vertexNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					auto fbxNormal = vertexNormal->GetDirectArray().GetAt(vertexID);
					normal = Utils::ToGLM3(fbxNormal);
				}
				else if (vertexNormal->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					int32_t id = vertexNormal->GetIndexArray().GetAt(vertexID);
					auto fbxNormal = vertexNormal->GetDirectArray().GetAt(id);
					normal = Utils::ToGLM3(fbxNormal);
				}
			}
		}

		return normal;
	}

	glm::vec3 FBXModelImporter::GetTangent(FbxMesh* mesh, int32_t vertexID, int32_t controlPoint)
	{
		glm::vec3 tangent(0, 0, 0);

		for (int i = 0; i < mesh->GetElementTangentCount(); ++i)
		{
			FbxGeometryElementTangent* vertexTangent = mesh->GetElementTangent(i);

			if (vertexTangent->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				if (vertexTangent->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					auto fbxTangent = vertexTangent->GetDirectArray().GetAt(controlPoint);
					tangent = Utils::ToGLM3(fbxTangent);
				}
				else if (vertexTangent->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					int id = vertexTangent->GetIndexArray().GetAt(controlPoint);
					auto fbxTangent = vertexTangent->GetDirectArray().GetAt(id);
					tangent = Utils::ToGLM3(fbxTangent);
				}
			}

			if (vertexTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				if (vertexTangent->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					auto fbxTangent = vertexTangent->GetDirectArray().GetAt(vertexID);
					tangent = Utils::ToGLM3(fbxTangent);
				}
				else if (vertexTangent->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					int id = vertexTangent->GetIndexArray().GetAt(vertexID);
					auto fbxTangent = vertexTangent->GetDirectArray().GetAt(id);
					tangent = Utils::ToGLM3(fbxTangent);
				}
			}
		}

		return tangent;
	}

	glm::vec2 FBXModelImporter::GetTexcoord(FbxMesh* mesh, int32_t vertexID, int32_t controlPoint, int32_t polygonPos)
	{
		glm::vec2 texCoord(0, 0);

		int32_t uvCount = mesh->GetElementUVCount();
		for (int32_t i = 0; i < uvCount; ++i)
		{
			FbxGeometryElementUV* vertexUV = mesh->GetElementUV(i);

			if (vertexUV->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				if (vertexUV->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					auto fbxTexCoord = vertexUV->GetDirectArray().GetAt(controlPoint);
					texCoord = Utils::ToGLM2(fbxTexCoord);
				}
				else if (vertexUV->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					int id = vertexUV->GetIndexArray().GetAt(controlPoint);
					auto fbxTexCoord = vertexUV->GetDirectArray().GetAt(id);
					texCoord = Utils::ToGLM2(fbxTexCoord);
				}
			}
			else if (vertexUV->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				int texCoordIndex = mesh->GetTextureUVIndex(polygonPos, vertexID);

				if (vertexUV->GetReferenceMode() == FbxGeometryElement::eDirect ||
					vertexUV->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					auto fbxTexCoord = vertexUV->GetDirectArray().GetAt(texCoordIndex);
					texCoord = Utils::ToGLM2(fbxTexCoord);
				}
			}
		}

		return texCoord;
	}

	void FBXModelImporter::GetBoneInfluences(FbxMesh* mesh)
	{
		fbxsdk::FbxSkin* rootSkin = nullptr;

		// Get the deformer from the mesh
		for (int i = 0; i < mesh->GetDeformerCount(); i++)
		{
			if (mesh->GetDeformer(i)->Is<fbxsdk::FbxSkin>())
			{
				rootSkin = (fbxsdk::FbxSkin*)mesh->GetDeformer(i);
				break;
			}
		}

		if (rootSkin == nullptr)
			return;

		// Resize the control point influences to match the number of control points
		m_RigData.ControlPointInfluences.clear();
		m_RigData.ControlPointInfluences.resize(mesh->GetControlPointsCount());

		FbxAMatrix geometryTransform = Utils::GetGeoTransformation(mesh->GetNode());

		// Iterate over each cluster in the skin
		int32_t numClusters = rootSkin->GetClusterCount();
		for (int32_t clusterIndex = 0; clusterIndex < numClusters; clusterIndex++)
		{
			// Get the cluster
			fbxsdk::FbxCluster* cluster = rootSkin->GetCluster(clusterIndex);

			// Get the linked node
			FbxNode* linkNode = cluster->GetLink();

			std::string boneName = linkNode->GetName();
			FbxAMatrix clusterTransform;
			FbxAMatrix clusterTransformLink;
			FbxAMatrix globalBIM;

			cluster->GetTransformMatrix(clusterTransform);
			cluster->GetTransformLinkMatrix(clusterTransformLink);

			if (m_Settings.ConvertToLH)
			{
				glm::mat4 geometry = Utils::ConvertLH(geometryTransform);
				glm::mat4 transform = Utils::ConvertLH(clusterTransform);
				glm::mat4 transformLink = Utils::ConvertLH(clusterTransformLink.Inverse());
				glm::mat4 inverseBindpose = transformLink * transform;
				inverseBindpose = inverseBindpose * geometry;
				m_RigData.Bones[boneName].inverseBindpose = inverseBindpose;
			}
			else
			{
				glm::mat4 geometry = Utils::ToGLM(geometryTransform);
				glm::mat4 transform = Utils::ToGLM(clusterTransform);
				glm::mat4 transformLink = Utils::ToGLM(clusterTransformLink.Inverse());
				glm::mat4 inverseBindpose = transformLink * transform;
				inverseBindpose = inverseBindpose * geometry;
				m_RigData.Bones[boneName].inverseBindpose = inverseBindpose;
			}

			// Get the number of control point indices
			int controlPointIndices = cluster->GetControlPointIndicesCount();

			// Iterate through each control point index
			for (int i = 0; i < controlPointIndices; i++)
			{
				// Get the control point index associated with this influence
				int controlPointIndex = cluster->GetControlPointIndices()[i];

				// Get the weight of the control point
				float weight = (float)cluster->GetControlPointWeights()[i];

				BoneInfluence& influence = m_RigData.ControlPointInfluences[controlPointIndex];
				float lowestWeight = weight;
				int lowComponent = -1;

				// Iterate through the control point influences in the set already and kick out the weakest influence
				for (int j = 0; j < 4; j++)
				{
					if (influence.Weights[j] < lowestWeight)
					{
						lowestWeight = influence.Weights[j];
						lowComponent = j;
					}
				}

				if (lowComponent != -1)
				{
					influence.Indices[lowComponent] = (float)m_RigData.Bones[boneName].Index;
					influence.Weights[lowComponent] = weight;
				}
			}

			// Handle animation for this bone
			// Get animation information
			// Now only supports one take
			FbxAnimStack* currAnimStack = m_CurrentScene->GetSrcObject<FbxAnimStack>(0);
			FbxString animStackName = currAnimStack->GetName();
			m_AnimationData.Name = animStackName.Buffer();
			FbxTakeInfo* takeInfo = m_CurrentScene->GetTakeInfo(animStackName);
			FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
			FbxTime end = takeInfo->mLocalTimeSpan.GetStop();

			m_AnimationData.Duration = takeInfo->mLocalTimeSpan.GetDuration().GetSecondDouble();

			for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames30); i <= end.GetFrameCount(FbxTime::eFrames30); ++i)
			{
				FbxTime currTime;
				currTime.SetFrame(i, FbxTime::eFrames30);
				double time = currTime.GetSecondDouble();
				FbxAMatrix currentTransformOffset = mesh->GetNode()->EvaluateLocalTransform(currTime) * geometryTransform;
				FbxAMatrix finalOffset = currentTransformOffset.Inverse() * linkNode->EvaluateLocalTransform(currTime);

				glm::mat4 finalPose;

				if (m_Settings.ConvertToLH)
					finalPose = Utils::ConvertLH(finalOffset);
				else
					finalPose = Utils::ToGLM(finalOffset);

				glm::vec3 translation;
				glm::vec3 scale;
				glm::quat rotation;
				glm::vec3 skew;
				glm::vec4 perspective;
				glm::decompose(finalPose, scale, rotation, translation, skew, perspective);

				auto& boneKeyframe = m_AnimationData.BoneKeyframes[boneName];
				boneKeyframe.SetBoneName(boneName);
				boneKeyframe.AddPositionKey(time, translation);
				boneKeyframe.AddRotationKey(time, rotation);
				boneKeyframe.AddScaleKey(time, scale);
			}
		}
	}
}