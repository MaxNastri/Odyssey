#pragma once
#include "Asset.h"
#include "GameObject.h"

namespace Odyssey
{
	class Prefab : public Asset
	{
		CLASS_DECLARATION(Odyssey, Prefab)
	public:
		Prefab() = default;
		Prefab(const Path& assetPath);

	public:
		void Save(GameObject& prefabInstance);
		void Load(GameObject& prefabInstance);

	private:
		void LoadFromDisk(const Path& assetPath);
		void SaveToDisk(const Path& assetPath);
	};
}