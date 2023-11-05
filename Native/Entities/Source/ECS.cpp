#include "ECS.h"
#include "GameObject.h"
#include "Transform.h"
#include "ComponentManager.h"
#include <Log.h>
#include <string>

namespace Odyssey::Entities
{
	void ECS::Create()
	{
		GameObject go;
		go.active = true;
		go.id = 118;

		Transform* transform = ComponentManager::AddComponent<Transform>(go);
		transform->position = Vector4(0, 1, 2, 3);
		Framework::Log::Info("Transform: "+ transform->position.ToString());

		Transform* againTransform = ComponentManager::GetComponent<Transform>(go);
		Framework::Log::Info("againTransform: " + transform->position.ToString());

		ComponentManager::RemoveComponent<Transform>(go);

		Transform* lastTransform = ComponentManager::GetComponent<Transform>(go);
		std::string msg = lastTransform == nullptr ? "true" : "false";
		Framework::Log::Info("Null check: " + msg);
	}
}