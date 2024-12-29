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
		void SetFill(float2 fill) { m_Fill = fill; }
		void SetBaseColor(float4 color) { m_BaseColor = color; }

	public:
		bool IsEnabled() { return m_Enabled; }
		Ref<Texture2D> GetSprite() { return m_Sprite; }
		float2 GetFill() { return m_Fill; }
		float4 GetBaseColor() { return m_BaseColor; }

	private:
		bool m_Enabled = true;
		GameObject m_GameObject;
		Ref<Texture2D> m_Sprite;
		float2 m_Fill = float2(1.0f);
		float4 m_BaseColor = float4(1.0f);
	};
}