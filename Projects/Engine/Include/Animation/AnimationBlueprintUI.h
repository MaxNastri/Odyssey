#pragma once
#include "AnimationProperty.hpp"

namespace Odyssey
{
	class AnimationBlueprint;
	struct AnimationBlueprintUI;

	struct PropertiesPanel
	{
	public:
		inline static std::string Window_Name = "Properties Panel";
	private:
		float2 Size = float2(400.0f, 800.0f);
		float2 MinSize = float2(50.0f, 50.0f);

	public:
		void Draw(AnimationBlueprint* blueprint, AnimationBlueprintUI& blueprintUI);
	};

	struct NodeInspectorPanel
	{
	public:
		inline static std::string Window_Name = "Node Inspector Panel";
	public:
		void Draw(AnimationBlueprint* blueprint, AnimationBlueprintUI& blueprintUI);

	private:
		float2 Size = float2(400.0f, 800.0f);
		float2 MinSize = float2(50.0f, 50.0f);
	};

	struct SelectPropertyMenu
	{
	public:
		inline static const uint32_t ID = 117;
		inline static const std::string Name = "Select Property Type Menu";

	public:
		void Open();
		void Draw(AnimationBlueprint* blueprint, AnimationBlueprintUI& blueprintUI);
	};

	struct AddPropertyMenu
	{
	public:
		inline static const uint32_t ID = 118;
		inline static const std::string Name = "Add Property Menu";

	private:
		char m_Buffer[128] = "";
		AnimationPropertyType PropertyType = AnimationPropertyType::None;

	public:
		void Open(AnimationPropertyType propertyType);
		void Draw(AnimationBlueprint* blueprint, AnimationBlueprintUI& blueprintUI);
	};

	struct CreateNodeMenu
	{
	public:
		inline static const uint32_t ID = 119;
		inline static const std::string Name = "Create Node Menu";

	public:
		void Draw(AnimationBlueprint* blueprint, AnimationBlueprintUI& blueprintUI);
	};

	struct AddAnimationLinkMenu
	{
	public:
		inline static const uint32_t ID = 120;
		inline static const std::string Name = "Add Animation Link Menu";

	public:
		void Open();
		void Draw(AnimationBlueprint* blueprint, AnimationBlueprintUI& blueprintUI);

	private:
		void Clear();

	private:
		char m_Buffer[128] = "";
		int32_t m_SelectedProperty = -1;
		int32_t m_SelectedComparisonOp = -1;
		RawBuffer m_InputValue;

		inline static std::array<std::string,5> Comparison_Op_Display =
		{
			"<", "<=", "=", ">", ">="
		};
	};

	struct AnimationBlueprintUI
	{
	public:
		void Draw(AnimationBlueprint* blueprint, AnimationBlueprintUI& blueprintUI);

	public:
		PropertiesPanel& GetPropertiesPanel() { return m_PropertiesPanel; }
		NodeInspectorPanel& GetNodeInspectorPanel() { return m_NodeInspectorPanel; }
		SelectPropertyMenu& GetSelectPropertyMenu() { return m_SelectPropertyMenu; }
		AddPropertyMenu& GetAddPropertyMenu() { return m_AddPropertyMenu; }
		CreateNodeMenu& GetCreateNodeMenu() { return m_CreateNodeMenu; }
		AddAnimationLinkMenu& GetAddAnimationLinkMenu() { return m_AddAnimationLinkMenu; }

	private:
		PropertiesPanel m_PropertiesPanel;
		NodeInspectorPanel m_NodeInspectorPanel;
		SelectPropertyMenu m_SelectPropertyMenu;
		AddPropertyMenu m_AddPropertyMenu;
		CreateNodeMenu m_CreateNodeMenu;
		AddAnimationLinkMenu m_AddAnimationLinkMenu;
	};
}