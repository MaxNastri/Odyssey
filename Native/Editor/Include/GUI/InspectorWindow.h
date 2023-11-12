#pragma once
#include <GUIElement.h>
#include <GameObject.h>
#include "TransformInspector.h"
#include "UserScriptInspector.h"

namespace Odyssey::Editor
{
	class InspectorWindow : public Graphics::GUIElement
	{
	public:
		InspectorWindow() = default;
		InspectorWindow(Entities::GameObject gameObject);

	public:
		virtual void Draw() override;
		void RefreshUserScripts();

	private:
		TransformInspector transformInspector;
		std::vector<UserScriptInspector> userScriptInspectors;
		bool open = true;
	};
}