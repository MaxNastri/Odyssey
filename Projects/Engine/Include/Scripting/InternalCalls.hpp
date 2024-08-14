#pragma once
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"

namespace Odyssey::InternalCalls
{
	uint64_t GameObject_Create()
	{
		if (Scene* activeScene = SceneManager::GetActiveScene())
		{
			GameObject gameObject = activeScene->CreateGameObject();
			return gameObject.GetGUID();
		}

		return (uint64_t)(-1);
	}
}