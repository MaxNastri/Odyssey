#pragma once
#include "Asset.h"
#include "FBXModelImporter.h"
#include "GLTFAssetImporter.h"

namespace Odyssey
{
	class SourceModel : public SourceAsset
	{
	public:
		SourceModel() = default;
		SourceModel(const Path& sourcePath);

	public:
		ModelAssetImporter* GetImporter() { return m_ModelImporter.get(); }

	private:
		std::unique_ptr<ModelAssetImporter> m_ModelImporter;
	};
}