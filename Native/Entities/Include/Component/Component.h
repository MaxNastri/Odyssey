#pragma once
#include "GameObject.h"
#include "Globals.h"
#include <Serialization.h>

namespace Odyssey::Entities
{
	// FWD Declarations
	class GameObject;

	class Component
	{
	public:
		virtual void Awake() { }
		virtual void Update() { }
		virtual void OnDestroy() { }
		virtual void Serialize(json& jsonObject) { to_json(jsonObject, *this); }
		virtual void Deserialize(const json& jsonObject) { from_json(jsonObject, *this); }

	public:
		bool active;
		GameObject gameObject;
		ODYSSEY_SERIALIZE(Component, active);
	};
}