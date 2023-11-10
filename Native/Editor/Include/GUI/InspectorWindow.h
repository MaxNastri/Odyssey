#pragma once
#include <GUIElement.h>
#include <GameObject.h>
#include "TransformInspector.h"

namespace Odyssey::Editor
{
	class InspectorWindow : public Graphics::GUIElement
	{
	public:
		InspectorWindow() = default;
		InspectorWindow(Entities::GameObject gameObject);

	public:
		virtual void Draw() override;

	private:
		TransformInspector transformInspector;
		bool open = true;
	};
}