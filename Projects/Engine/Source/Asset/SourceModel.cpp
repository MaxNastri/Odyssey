#include "SourceModel.h"

namespace Odyssey
{
	SourceModel::SourceModel(const Path& sourcePath)
		: SourceAsset(sourcePath)
	{
		// TODO: Add validation
		if (sourcePath.extension() == ".fbx")
			m_FBXImporter.Import(sourcePath);
		else
			m_Importer.Import(sourcePath);
	}
}