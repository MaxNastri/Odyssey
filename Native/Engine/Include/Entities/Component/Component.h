#pragma once
#include "Globals.h"
#include "AssetSerializer.h"

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
		virtual void Serialize(SerializationNode& node) { }
		virtual void Deserialize(SerializationNode& node) { }

	public:
		void SetGameObject(GameObject* go)
		{
			gameObject = go;
		}

	public:
		bool active;
		GameObject* gameObject;
	};
}