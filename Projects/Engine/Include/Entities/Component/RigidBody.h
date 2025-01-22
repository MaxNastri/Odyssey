#pragma once
#include "GameObject.h"
#include "AssetSerializer.h"
#include <Jolt/Physics/Body/BodyID.h>

namespace Odyssey
{
	class RigidBody
	{
		CLASS_DECLARATION(Odyssey, RigidBody)
	public:
		RigidBody() = default;
		RigidBody(const GameObject& gameObject);
		RigidBody(const GameObject& gameObject, SerializationNode& node);

	public:
		void Awake();
		void Update();
		void Destroy();
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		void SetEnabled(bool enabled);

	public:
		bool IsEnabled() { return m_Enabled; }
		BodyID GetBodyID() { return m_BodyID; }

	private:
		bool m_Enabled = true;
		GameObject m_GameObject;
		BodyID m_BodyID;
	};
}