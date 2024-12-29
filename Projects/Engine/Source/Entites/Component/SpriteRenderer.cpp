#include "SpriteRenderer.h"
#include "AssetManager.h"

namespace Odyssey
{
	SpriteRenderer::SpriteRenderer(const GameObject& gameObject)
		: m_GameObject(gameObject)
	{

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
	}

	void SpriteRenderer::Deserialize(SerializationNode& node)
	{
		GUID sprite;

		node.ReadData("Enabled", m_Enabled);
		node.ReadData("Sprite", sprite.Ref());
		node.ReadData("Base Color", m_BaseColor);
		node.ReadData("Fill", m_Fill);

		if (sprite)
			SetSprite(sprite);
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
}