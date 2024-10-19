#include "AnimationBlueprint.h"
#include "widgets.h"
#include "AnimationNodes.h"
#include "AnimationProperty.hpp"
#include "Input.h"

namespace Odyssey
{
	AnimationBlueprint::AnimationBlueprint()
		: Blueprint()
	{
		m_Builder = std::make_shared<BlueprintBuilder>(this);
		m_Builder->OverrideCreateNodeMenu(CreateNodeMenu::Name, CreateNodeMenu::ID);

		AddProperty("Jumping", AnimationPropertyType::Bool);
		AddProperty("Speed", AnimationPropertyType::Float);
		AddProperty("Hit Count", AnimationPropertyType::Int);
		AddProperty("Swing Weapon", AnimationPropertyType::Trigger);

		for (auto& animProperty : m_Properties)
		{
			m_PropertyMap[animProperty->Name] = animProperty;
		}

		// Build nodes
		BuildNodes();
	}

	void AnimationBlueprint::Update()
	{
		ClearTriggers();
	}

	void AnimationBlueprint::Draw()
	{
		m_Builder->SetEditor();

		m_UI.Draw(this, m_UI);

		ImGui::SameLine(0.0f, 12.0f);

		// Begin building the UI
		m_Builder->Begin();

		// Draw the blueprint
		m_Builder->DrawBlueprint();

		// End building the UI
		m_Builder->End();
	}

	void AnimationBlueprint::OnNodeAdded(std::shared_ptr<Node> node)
	{
		m_Builder->ConnectNewNode(node.get());
	}

	void AnimationBlueprint::AddLink(Pin* start, Pin* end)
	{
		m_PendingLinkStart = start;
		m_PendingLinkEnd = end;
		m_UI.GetAddAnimationLinkMenu().Open();
	}

	void AnimationBlueprint::ConfirmPendingLink()
	{
		Blueprint::AddLink(m_PendingLinkStart, m_PendingLinkEnd);
	}

	void AnimationBlueprint::ClearPendingLink()
	{
		m_PendingLinkStart = m_PendingLinkEnd = nullptr;
	}

	void AnimationBlueprint::AddProperty(std::string_view name, AnimationPropertyType type)
	{
		m_Properties.push_back(std::make_shared<AnimationProperty>(name, type));
	}

	bool AnimationBlueprint::SetBool(const std::string& name, bool value)
	{
		if (m_PropertyMap.contains(name))
		{
			m_PropertyMap[name]->ValueBuffer.Write(&value);
			return true;
		}
		return false;
	}

	bool AnimationBlueprint::SetFloat(const std::string& name, float value)
	{
		if (m_PropertyMap.contains(name))
		{
			m_PropertyMap[name]->ValueBuffer.Write(&value);
			return true;
		}
		return false;
	}

	bool AnimationBlueprint::SetInt(const std::string& name, int32_t value)
	{
		if (m_PropertyMap.contains(name))
		{
			m_PropertyMap[name]->ValueBuffer.Write(&value);
			return true;
		}
		return false;
	}

	bool AnimationBlueprint::SetTrigger(const std::string& name)
	{
		if (m_PropertyMap.contains(name))
		{
			bool trigger = true;
			m_PropertyMap[name]->ValueBuffer.Write(&trigger);
			return true;
		}
		return false;
	}

	void AnimationBlueprint::ClearTriggers()
	{
		const bool clearTrigger = false;

		for (auto& property : m_Properties)
		{
			if (property->Type == AnimationPropertyType::Trigger)
			{
				property->ValueBuffer.Write(&clearTrigger);
			}
		}
	}

	void AnimationBlueprint::EvalulateGraph()
	{

	}
}