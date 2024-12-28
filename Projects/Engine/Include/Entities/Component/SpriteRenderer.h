#pragma once
#include "AssetSerializer.h"
#include "GameObject.h"
#include "Texture2D.h"

namespace Odyssey
{
	class SpriteRenderer
	{
		CLASS_DECLARATION(Odyssey, SpriteRenderer)
	public:
		SpriteRenderer() = default;
		SpriteRenderer(const GameObject& gameObject);

	public:
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		void SetEnabled(bool enabled);
		void SetSprite(GUID guid);

	public:
		bool IsEnabled() { return m_Enabled; }
		Ref<Texture2D> GetSprite() { return m_Sprite; }

	private:
		bool m_Enabled = true;
		GameObject m_GameObject;
		Ref<Texture2D> m_Sprite;
	};
}