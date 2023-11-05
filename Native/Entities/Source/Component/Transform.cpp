#include "Transform.h"
#include <Log.h>

namespace Odyssey::Entities
{
	CLASS_DEFINITION(Odyssey.Entities, Transform);

	void Transform::Awake()

	{
		Framework::Log::Info("Transform Logging from Awake: " + position.ToString());
	}

	void Transform::Update()
	{
		position.x += 1;
		Framework::Log::Info("Transform Logging from Update: " + position.ToString());
	}

	void Transform::OnDestroy()
	{
		Framework::Log::Info("Transform Logging from OnDestroy: " + position.ToString());
	}

	void Transform::Serialize(json& jsonObject)
	{
		json componentJson;
		to_json(componentJson, *this);
		jsonObject += { "Component", componentJson};
	}

	void Transform::Deserialize(const json& jsonObject)
	{
		from_json(jsonObject, *this);
	}
}