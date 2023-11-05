#pragma once
#include "Component.h"
#include <Vector4.h>
#include <Serialization.h>

namespace Odyssey::Entities
{
	class Transform : public Component
	{
	public:
		virtual void Awake() override;
		virtual void Update() override;
		virtual void OnDestroy() override;
		virtual void Serialize(json& jsonObject) override;
	public:
		Vector4 position;
		ODYSSEY_SERIALIZE(Transform, position);
		CLASS_DECLARATION(Transform);
	};
}