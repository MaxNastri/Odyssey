#include "ECS.h"
#include "GameObject.h"
#include "Transform.h"
#include "ComponentManager.h"
#include <Log.h>
#include <string>
#include <fstream>

namespace Odyssey::Entities
{
	Scene ECS::scene;

	void ECS::Create()
	{
		GameObject go = scene.CreateGameObject();
		GameObject go2 = scene.CreateGameObject();
		//Transform* transform = ComponentManager::AddComponent<Transform>(go);
		ComponentManager::AddComponentByName(go, "Odyssey.Entities.Transform");
		Transform* transform = ComponentManager::GetComponent<Transform>(go);
		transform->position = Vector4(0, 1, 2, 3);

		scene.Serialize("scene.json");

		scene.Awake();
	}

	void ECS::Load()
	{
		scene.Deserialize("scene.json");
	}

	void ECS::Update()
	{
		scene.Update();
	}
}