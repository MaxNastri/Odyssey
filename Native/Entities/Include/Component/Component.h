#pragma once
#include "GameObject.h"

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

	public:
		bool active;
		GameObject gameObject;
	};
}