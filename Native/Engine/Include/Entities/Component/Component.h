#pragma once
#include "GameObject.h"
#include "Globals.h"

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
	};
}