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
		InspectorWindow(RefHandle<GameObject> gameObject);

	public:
		virtual void Draw() override;

	public:
		void SetGameObject(RefHandle<GameObject> gameObject);
		void RefreshUserScripts();

	private:
		std::vector<std::unique_ptr<GUIElement>> inspectors;
		TransformInspector transformInspector;
		CameraInspector cameraInspector;
		std::vector<UserScriptInspector> userScriptInspectors;
		bool open = true;
	};
}