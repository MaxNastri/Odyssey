#include "SourceModel.h"
#include "Log.h"
#include "GLTFAssetImporter.h"
#include "FBXAssetImporter.h"

namespace Odyssey
{
	SourceModel::SourceModel(const Path& sourcePath)
		: SourceAsset(sourcePath)
	{
		if (sourcePath.extension() == ".glb" || sourcePath.extension() == ".gltf")
			m_ModelImporter = std::make_unique<GLTFAssetImporter>();
		else if (sourcePath.extension() == ".fbx")
			m_ModelImporter = std::make_unique<FBXAssetImporter>();

		if (!m_ModelImporter || !m_ModelImporter->Import(sourcePath))
			Log::Error(std::format("Failed to import model: {}", sourcePath.string()));
	}
}