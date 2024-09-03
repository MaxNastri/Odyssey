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
		FBXModelImporter& GetFBXImporter() { return m_FBXImporter; }
		GLTFAssetImporter& GetGLTFImporter() { return m_gltfImporer; }

	private:
		FBXModelImporter m_FBXImporter;
		GLTFAssetImporter m_gltfImporer;
	};
}