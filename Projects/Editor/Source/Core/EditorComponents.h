#pragma once

namespace Odyssey
{
	class EditorPropertiesComponent
	{
	public:
		EditorPropertiesComponent() = default;
		EditorPropertiesComponent(const GameObject& gameObject) { }

	public:
		void Awake() { }
	public:
		bool ShowInHierarchy = true;
	};
}