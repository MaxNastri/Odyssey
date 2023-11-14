#pragma once
#include "GameObject.h"
#include "Globals.h"
#include <Serialization.h>

namespace Odyssey
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
		virtual void Serialize(ryml::NodeRef& node) { }
		virtual void Deserialize(ryml::ConstNodeRef node) { }

	public:
		void SetGameObject(GameObject go)
		{
			gameObject = go;
		}

	public:
		bool active;
		GameObject gameObject;
		std::string uuid;
		ODYSSEY_SERIALIZE(Component, active);
	};
}