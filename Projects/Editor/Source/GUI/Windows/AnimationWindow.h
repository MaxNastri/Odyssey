#pragma once
#include "AnimationBlueprint.h"
#include "DockableWindow.h"
#include "PropertyDrawers.h"
#include "Ref.h"

namespace Odyssey
{
	namespace Rune
	{
		class BlueprintBuilder;
	}

	class AnimationWindow : public DockableWindow
	{
	public:
		AnimationWindow(size_t windowID);
		void Destroy();

	public:
		virtual void Update() override;
		virtual bool Draw() override;
		virtual void OnWindowClose() override;

	private:
		void CreateBuilder();

	private:
		void DrawPropertiesPanel();
		void DrawNodeInspectorPanel();

		void DrawSelectPropertyMenu();
		void DrawAddPropertyMenu();
		void DrawCreateNodeMenu();
		void DrawAddLinkMenu();

	private:
		void DrawAnimationCondition(Ref<AnimationCondition>& condition);

	private:
		void OpenSelectPropertyMenu();
		void OpenAddPropertyMenu(AnimationPropertyType propertyType);
		void OpenAddLinkMenu();

	private:
		Ref<AnimationBlueprint> m_Blueprint;
		Ref<BlueprintBuilder> m_Builder;
		uint32_t m_DockspaceID = 0;

	private: // Node inspector panel
		Ref<AnimationState> m_AnimationState;
		Ref<AnimationLink> m_AnimationLink;

	private: // Animation State
		StringDrawer m_StateNameDrawer;
		AssetFieldDrawer m_AnimationClipDrawer;

	private: // Animation Link
		EnumDrawer<ComparisonOp> m_ComparisonDrawer;
		Ref<PropertyDrawer> m_LinkValueDrawer;

	private: // Add property menu
		char m_AddPropertyBuffer[128] = "";
		AnimationPropertyType m_AddPropertyType;

	private: // Add link menu
		char m_AddLinkBuffer[128] = "";
		int32_t m_SelectedProperty = -1;
		int32_t m_SelectedComparisonOp = -1;
		RawBuffer m_AddLinkPropertyValue;

	private:
		inline static const uint32_t Select_Property_Menu_ID = 117;
		inline static const uint32_t Add_Property_Menu_ID = 118;
		inline static const uint32_t Create_Node_Menu_ID = 119;
		inline static const uint32_t Add_Link_Menu_ID = 120;
		inline static const char* Select_Property_Menu_Name = "Select Property Type Menu";
		inline static const char* Add_Property_Menu_Name = "Add Property Menu";
		inline static const char* Create_Node_Menu_Name = "Create Node Menu";
		inline static const char* Add_Link_Menu_Name = "Add Animation Link Menu";
	};
}