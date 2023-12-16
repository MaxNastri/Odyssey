#pragma once
#include "GUIElement.h"

namespace Odyssey
{
	class GameObject;

	class MeshRendererInspector : public GUIElement
	{
	public:
		MeshRendererInspector() = default;
		MeshRendererInspector(GameObject* gameObject);

	public:
		virtual void Draw() override;

	private:
		GameObject* m_GameObject;

	};
}