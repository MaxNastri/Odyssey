#pragma once
#include "ModelAssetImporter.h"

namespace Odyssey
{
	class FBXAssetImporter : public ModelAssetImporter
	{
	public:
		FBXAssetImporter() = default;

	public:
		virtual bool Import(const Path& modelPath) override;
	};
}