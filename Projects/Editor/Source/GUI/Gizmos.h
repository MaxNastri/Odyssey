#pragma once
#include "EventSystem.h"

namespace Odyssey
{
	struct GizmoInfo
	{
	public:
		bool Enabled = false;
		std::function<void(bool)> OnGizmoModified;
	};

	struct SceneModifiedEvent;
	struct SceneLoadedEvent;

	class Gizmos
	{
	public:
		static void Init();

	public:
		static std::map<std::string, GizmoInfo>& GetGizmoInfo() { return m_Gizmos; }

	private:
		static void OnSceneModified(SceneModifiedEvent* onEvent);
		static void OnSceneLoaded(SceneLoadedEvent* onEvent);

	private:
		inline static std::map<std::string, GizmoInfo> m_Gizmos;
		inline static Ref<IEventListener> m_OnSceneModified;
		inline static Ref<IEventListener> m_OnSceneLoaded;
	};
}