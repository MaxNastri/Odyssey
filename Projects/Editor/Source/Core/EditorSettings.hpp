#pragma once

namespace Odyssey
{
	class EditorSettings
	{
	public:
		Path EditorAssetRegistry;
		std::set<std::string> AssetExtensions;
		std::unordered_map<std::string, std::string> SourceAssetBindings;
	};
}