#pragma once

namespace Odyssey
{
	class GUIElement
	{
	public:
		virtual void Update() { };
		virtual void Draw() { };
		virtual void Reload() { }
	};
}