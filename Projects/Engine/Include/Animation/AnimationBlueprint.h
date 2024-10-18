#pragma once
#include "Blueprint.h"
#include "RawBuffer.hpp"

namespace Odyssey
{
	using namespace Rune;

	struct Property
	{
		std::string Name;
		RawBuffer ValueBuffer;
	};

	class AnimationBlueprint : public Rune::Blueprint
	{
	public:
		AnimationBlueprint();

	public:
		virtual void Update() override;

	public:
		bool SetProperty(const std::string& name, bool value);

	private:
		void DrawPropertiesPanel();

	private:
		std::map<std::string, Property> m_Properties;

	private:
		struct PropertiesPanel
		{
		public:
			float2 Size = float2(400.0f, 800.0f);
			float2 MinSize = float2(50.0f, 50.0f);
		};
		PropertiesPanel m_PropertiesPanel;

	private:
		const std::string Create_Node_Menu = "My Create Node";
		uint32_t m_CreateNodeMenuID = 117;
	};
}