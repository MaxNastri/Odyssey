#pragma once
#include "Asset.h"
#include "ModelImporter.h"
#include "FBXModelImporter.h"

namespace Odyssey
{
	class SourceModel : public SourceAsset
	{
	public:
		SourceModel() = default;
		SourceModel(const Path& sourcePath);

	public:
		ModelImporter& GetImporter() { return m_Importer; }
		FBXModelImporter& GetFBXImporter() { return m_FBXImporter; }

	private:
		ModelImporter m_Importer;
		FBXModelImporter m_FBXImporter;
	};
}