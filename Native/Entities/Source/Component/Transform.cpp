#include "Transform.h"
#include <Log.h>

namespace Odyssey::Entities
{
	CLASS_DEFINITION(Odyssey.Entities, Transform);

	void Transform::Awake()
	{
	}

	void Transform::Update()
	{
		Framework::Log::Info("[Transform] Position: " + position.ToString());
	}

	void Transform::OnDestroy()
	{
		
	}

	void Transform::Serialize(json& jsonObject)
	{
		json componentJson;
		to_json(componentJson, *this);
		jsonObject += { "Component." + Transform::ClassName, componentJson};
	}

	void Transform::Deserialize(const json& jsonObject)
	{
		from_json(jsonObject, *this);
	}
}