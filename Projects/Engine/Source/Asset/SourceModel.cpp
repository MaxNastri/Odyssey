#include "SourceModel.h"

namespace Odyssey
{
	SourceModel::SourceModel(const Path& sourcePath)
		: SourceAsset(sourcePath)
	{
		if (sourcePath.extension() == ".fbx")
			m_FBXImporter.Import(sourcePath);
		else if (sourcePath.extension() == ".glb" || sourcePath.extension() == ".gltf")
			m_gltfImporer.Import(sourcePath);
	}
}