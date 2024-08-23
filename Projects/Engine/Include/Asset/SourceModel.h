#pragma once
#include "Asset.h"
#include "ModelImporter.h"

namespace Odyssey
{
	class SourceModel : public SourceAsset
	{
	public:
		SourceModel() = default;
		SourceModel(const Path& sourcePath);

	public:
		ModelImporter& GetImporter() { return m_Importer; }

	private:
		ModelImporter m_Importer;
	};
}