#include "SourceModel.h"
#include "Logger.h"

namespace Odyssey
{
	SourceModel::SourceModel(const Path& sourcePath)
		: SourceAsset(sourcePath)
	{
		if (sourcePath.extension() == ".glb" || sourcePath.extension() == ".gltf")
			m_ModelImporter = std::make_unique<GLTFAssetImporter>();

		if (!m_ModelImporter || !m_ModelImporter->Import(sourcePath))
			Logger::LogError(std::format("Failed to import model: {}", sourcePath.string()));
	}
}