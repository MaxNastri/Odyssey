#pragma once
#include <GUIElement.h>
#include <GameObject.h>
#include "TransformInspector.h"
#include "CameraInspector.h"
#include "UserScriptInspector.h"

namespace Odyssey
{
	class InspectorWindow : public GUIElement
	{
	public:
		InspectorWindow() = default;
		InspectorWindow(GameObject gameObject);

	public:
		virtual void Draw() override;
		void RefreshUserScripts();

	private:
		TransformInspector transformInspector;
		CameraInspector cameraInspector;
		std::vector<UserScriptInspector> userScriptInspectors;
		bool open = true;
	};
}