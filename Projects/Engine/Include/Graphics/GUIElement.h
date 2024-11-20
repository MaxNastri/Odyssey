#pragma once

namespace Odyssey
{
	class GUIElement
	{
	public:
		virtual void Update() { };
		virtual bool Draw() { return false; }
		virtual void Reload() { }
	};
}