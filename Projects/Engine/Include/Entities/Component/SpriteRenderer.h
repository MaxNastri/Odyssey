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
		enum class AnchorPosition
		{
			BottomLeft = 0,
			BottomRight = 1,
			TopLeft = 2,
			TopRight = 3,
		};
	public:
		SpriteRenderer() = default;
		SpriteRenderer(const GameObject& gameObject);

	public:
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		void SetEnabled(bool enabled);
		void SetSprite(GUID guid);
		void SetFill(float2 fill);
		void SetBaseColor(float4 color) { m_BaseColor = color; }
		void SetAnchor(AnchorPosition anchor) { m_AnchorPoint = anchor; }

	public:
		bool IsEnabled() { return m_Enabled; }
		Ref<Texture2D> GetSprite() { return m_Sprite; }
		float2 GetFill() { return m_Fill; }
		float4 GetBaseColor() { return m_BaseColor; }
		AnchorPosition GetAnchor() { return m_AnchorPoint; }

	private:
		bool m_Enabled = true;
		GameObject m_GameObject;
		Ref<Texture2D> m_Sprite;
		float2 m_Fill = float2(1.0f);
		float4 m_BaseColor = float4(1.0f);
		AnchorPosition m_AnchorPoint = AnchorPosition::BottomLeft;
	};
}