#pragma once
#include "AnimationBlueprint.h"
#include "AnimationProperty.hpp"
#include "PropertyDrawers.h"

namespace Odyssey
{
	namespace Rune
	{
		class BlueprintBuilder;
	}
	using namespace Rune;

	class AnimationState;
	class AnimationBlueprint;
	struct AnimationBlueprintUI;

	struct PropertiesPanel
	{
	public:
		void Draw(AnimationBlueprint* blueprint, BlueprintBuilder* builder, AnimationBlueprintUI* blueprintUI);

	public:
		inline static const std::string Window_Name = "Properties Panel";

	private:
		float2 m_Size = float2(400.0f, 800.0f);
		float2 m_MinSize = float2(50.0f, 50.0f);
	};

	struct NodeInspectorPanel
	{
	public:
		void Draw(AnimationBlueprint* blueprint, BlueprintBuilder* builder, AnimationBlueprintUI* blueprintUI);

	private:
		void OnAnimationClipChanged(GUID guid);

	public:
		inline static const std::string Window_Name = "Node Inspector Panel";

	private:
		float2 m_Size = float2(400.0f, 800.0f);
		float2 m_MinSize = float2(50.0f, 50.0f);
		std::shared_ptr<AnimationState> m_AnimationState;
		AssetFieldDrawer m_AnimationClipDrawer;
	};

	struct SelectPropertyMenu
	{

	public:
		void Open();
		void Draw(AnimationBlueprint* blueprint, BlueprintBuilder* builder, AnimationBlueprintUI* blueprintUI);

	public:
		inline static const uint32_t ID = 117;
		inline static const std::string Menu_Name = "Select Property Type Menu";
	};

	struct AddPropertyMenu
	{
	public:
		void Open(AnimationPropertyType propertyType);
		void Draw(AnimationBlueprint* blueprint, BlueprintBuilder* builder, AnimationBlueprintUI* blueprintUI);

	public:
		inline static const uint32_t ID = 118;
		inline static const std::string Menu_Name = "Add Property Menu";

	private:
		char m_Buffer[128] = "";
		AnimationPropertyType m_PropertyType = AnimationPropertyType::None;
	};

	struct CreateNodeMenu
	{
	public:
		void Draw(AnimationBlueprint* blueprint, BlueprintBuilder* builder, AnimationBlueprintUI* blueprintUI);

	public:
		inline static const uint32_t ID = 119;
		inline static const std::string Menu_Name = "Create Node Menu";
	};

	struct AddAnimationLinkMenu
	{
	public:
		void Open();
		void Draw(AnimationBlueprint* blueprint, BlueprintBuilder* builder, AnimationBlueprintUI* blueprintUI);

	private:
		void Clear();

	public:
		inline static const uint32_t ID = 120;
		inline static const std::string Menu_Name = "Add Animation Link Menu";

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
		void Draw(AnimationBlueprint* blueprint, BlueprintBuilder* builder);

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