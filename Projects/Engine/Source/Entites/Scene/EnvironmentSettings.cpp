#include "EnvironmentSettings.h"
#include "AssetManager.h"

namespace Odyssey
{
	void EnvironmentSettings::SetSkybox(GUID guid)
	{
		if (guid)
			Skybox = AssetManager::LoadAsset<Cubemap>(guid);
		else
			Skybox.Reset();
	}
}