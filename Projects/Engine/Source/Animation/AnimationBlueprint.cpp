#include "AnimationBlueprint.h"
#include "AnimationState.h"
#include "AnimationNodes.h"

namespace Odyssey
{
	AnimationBlueprint::AnimationBlueprint()
		: Blueprint()
	{
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

	void AnimationBlueprint::AddAnimationState(std::string name)
	{
		auto state = std::make_shared<AnimationState>();
		auto node = AddNode<AnimationStateNode>(name, state);

		m_States[node->ID] = state;
	}

	std::shared_ptr<AnimationState> AnimationBlueprint::GetAnimationState(NodeID nodeID)
	{
		if (m_States.contains(nodeID))
			return m_States[nodeID];

		return nullptr;
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

	void AnimationBlueprint::OnNodeAdded(std::shared_ptr<Node> node)
	{
		auto state = std::make_shared<AnimationState>();
		if (auto animationStateNode = std::static_pointer_cast<AnimationStateNode>(node))
			animationStateNode->SetAnimationState(state);

		m_States[node->ID] = state;
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