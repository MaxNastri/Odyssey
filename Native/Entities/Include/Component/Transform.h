#pragma once
#include "Component.h"
#include <Vector4.h>
#include <Log.h>

namespace Odyssey::Entities
{
	class MeshRenderer : public Component
	{
	public:
		virtual void Awake() override
		{
			Framework::Log::Info("MeshRenderer Logging from Awake: " + rotation.ToString());
		}
		virtual void Update() override
		{
			rotation.x += 1;
			Framework::Log::Info("MeshRenderer Logging from Update: " + rotation.ToString());
		}
		virtual void OnDestroy() override
		{
			Framework::Log::Info("MeshRenderer Logging from OnDestroy: " + rotation.ToString());
		}

	private:
		Vector3 rotation;
	};

	class Transform : public Component
	{
	public:
		virtual void Awake() override
		{
			Framework::Log::Info("Transform Logging from Awake: " + position.ToString());
		}
		virtual void Update() override
		{
			position.x += 1;
			Framework::Log::Info("Transform Logging from Update: " + position.ToString());
		}
		virtual void OnDestroy() override
		{
			Framework::Log::Info("Transform Logging from OnDestroy: " + position.ToString());
		}

	public:
		Vector4 position;
	};
}