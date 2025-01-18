#include "SpriteRenderer.h"
#include "AssetManager.h"
#include "Preferences.h"
#include "Enum.h"

namespace Odyssey
{
	SpriteRenderer::SpriteRenderer(const GameObject& gameObject)
		: m_GameObject(gameObject)
	{
		m_Sprite = AssetManager::LoadAsset<Texture2D>(Preferences::GetDefaultSprite());
	}

	SpriteRenderer::SpriteRenderer(const GameObject& gameObject, SerializationNode& node)
		: m_GameObject(gameObject)
	{
		Deserialize(node);
	}

	void SpriteRenderer::Serialize(SerializationNode& node)
	{
		uint64_t sprite = m_Sprite ? m_Sprite->GetGUID().Ref() : 0;

		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();
		componentNode.WriteData("Type", SpriteRenderer::Type);
		componentNode.WriteData("Enabled", m_Enabled);
		componentNode.WriteData("Sprite", sprite);
		componentNode.WriteData("Base Color", m_BaseColor);
		componentNode.WriteData("Fill", m_Fill);
		componentNode.WriteData("Anchor Position", Enum::ToString<AnchorPosition>(m_AnchorPoint));
	}

	void SpriteRenderer::Deserialize(SerializationNode& node)
	{
		GUID sprite;
		std::string anchor;

		node.ReadData("Enabled", m_Enabled);
		node.ReadData("Sprite", sprite.Ref());
		node.ReadData("Base Color", m_BaseColor);
		node.ReadData("Fill", m_Fill);
		node.ReadData("Anchor Position", anchor);

		if (sprite)
			SetSprite(sprite);

		if (!anchor.empty())
			m_AnchorPoint = Enum::ToEnum<AnchorPosition>(anchor);
	}

	void SpriteRenderer::SetEnabled(bool enabled)
	{
		m_Enabled = enabled;
	}

	void SpriteRenderer::SetSprite(GUID guid)
	{
		if (guid)
			m_Sprite = AssetManager::LoadAsset<Texture2D>(guid);
		else
			m_Sprite.Reset();
	}

	void SpriteRenderer::SetFill(float2 fill)
	{
		m_Fill = glm::clamp(fill, float2(0.0f), float2(1.0f));
	}
}