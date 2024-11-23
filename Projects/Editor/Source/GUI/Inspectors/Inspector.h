#pragma once
#include "Ref.h"
#include "EditorEnums.h"
#include "GUIElement.h"

namespace Odyssey
{
	class Inspector : public GUIElement
	{
	public:
		virtual bool Draw() { return false; }
	};
}