#include "ECS.h"
#include "GameObject.h"
#include "Transform.h"
#include "ComponentManager.h"
#include <Log.h>
#include <string>

namespace Odyssey::Entities
{
	Scene ECS::scene;

	void ECS::Create()
	{
		for (int i = 0; i < 1000; ++i)
		{
			GameObject go = scene.CreateGameObject();
			Transform* transform = ComponentManager::AddComponent<Transform>(go);
			transform->position = Vector4(0, 1, 2, 3);

			MeshRenderer* mr = ComponentManager::AddComponent<MeshRenderer>(go);

			scene.DestroyGameObject(go);
		}

		for (int i = 0; i < 750; ++i)
		{
			GameObject go = scene.CreateGameObject();
			Transform* transform = ComponentManager::AddComponent<Transform>(go);
			transform->position = Vector4(3,2,1,0);

			MeshRenderer* mr = ComponentManager::AddComponent<MeshRenderer>(go);
		}

		scene.Awake();
	}

	void ECS::Update()
	{
		scene.Update();
	}
}