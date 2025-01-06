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
		Prefab(const Path& assetPath, GameObject& instance);

	public:
		virtual void Save() override { }
		void Save(GameObject& prefabInstance);
		GameObject LoadInstance();
	};
}