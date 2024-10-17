#include "SourceModel.h"
#include "Log.h"

namespace Odyssey
{
	SourceModel::SourceModel(const Path& sourcePath)
		: SourceAsset(sourcePath)
	{
		if (sourcePath.extension() == ".glb" || sourcePath.extension() == ".gltf")
			m_ModelImporter = std::make_unique<GLTFAssetImporter>();

		if (!m_ModelImporter || !m_ModelImporter->Import(sourcePath))
			Log::Error(std::format("Failed to import model: {}", sourcePath.string()));
	}
}