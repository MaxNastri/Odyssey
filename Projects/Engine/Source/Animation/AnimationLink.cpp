#include "AnimationLink.h"
#include "AnimationState.h"
#include "Enum.h"

namespace Odyssey
{
	AnimationCondition::AnimationCondition(Ref<AnimationProperty> property, ComparisonOp comparison, const RawBuffer& targetValue, float blendTime)
	{
		m_Property = property;
		m_ComparisonOp = comparison;
		RawBuffer::Copy(m_TargetValue, targetValue);
		m_BlendTime = blendTime;
	}

	bool AnimationCondition::Evaluate()
	{
		switch (m_ComparisonOp)
		{
			case Odyssey::ComparisonOp::Less:
			{
				switch (m_Property->Type)
				{
					case AnimationPropertyType::Float:
						return m_Property->ValueBuffer.Read<float>() < m_TargetValue.Read<float>();
					case AnimationPropertyType::Int:
						return m_Property->ValueBuffer.Read<int32_t>() < m_TargetValue.Read<int32_t>();
					case AnimationPropertyType::Bool:
					case AnimationPropertyType::Trigger:
						return m_Property->ValueBuffer.Read<bool>() < m_TargetValue.Read<bool>();
				}
				break;
			}
			case Odyssey::ComparisonOp::LessOrEqual:
			{
				switch (m_Property->Type)
				{
					case AnimationPropertyType::Float:
						return m_Property->ValueBuffer.Read<float>() <= m_TargetValue.Read<float>();
					case AnimationPropertyType::Int:
						return m_Property->ValueBuffer.Read<int32_t>() <= m_TargetValue.Read<int32_t>();
					case AnimationPropertyType::Bool:
					case AnimationPropertyType::Trigger:
						return m_Property->ValueBuffer.Read<bool>() <= m_TargetValue.Read<bool>();
				}
				break;
			}
			case Odyssey::ComparisonOp::Equal:
			{
				switch (m_Property->Type)
				{
					case AnimationPropertyType::Float:
						return m_Property->ValueBuffer.Read<float>() == m_TargetValue.Read<float>();
					case AnimationPropertyType::Int:
						return m_Property->ValueBuffer.Read<int32_t>() == m_TargetValue.Read<int32_t>();
					case AnimationPropertyType::Bool:
					case AnimationPropertyType::Trigger:
						return m_Property->ValueBuffer.Read<bool>() == m_TargetValue.Read<bool>();
				}
				break;
			}
			case Odyssey::ComparisonOp::Greater:
			{
				switch (m_Property->Type)
				{
					case AnimationPropertyType::Float:
						return m_Property->ValueBuffer.Read<float>() > m_TargetValue.Read<float>();
					case AnimationPropertyType::Int:
						return m_Property->ValueBuffer.Read<int32_t>() > m_TargetValue.Read<int32_t>();
					case AnimationPropertyType::Bool:
					case AnimationPropertyType::Trigger:
						return m_Property->ValueBuffer.Read<bool>() > m_TargetValue.Read<bool>();
				}
				break;
			}
			case Odyssey::ComparisonOp::GreaterOrEqual:
			{
				switch (m_Property->Type)
				{
					case AnimationPropertyType::Float:
						return m_Property->ValueBuffer.Read<float>() >= m_TargetValue.Read<float>();
					case AnimationPropertyType::Int:
						return m_Property->ValueBuffer.Read<int32_t>() >= m_TargetValue.Read<int32_t>();
					case AnimationPropertyType::Bool:
					case AnimationPropertyType::Trigger:
						return m_Property->ValueBuffer.Read<bool>() >= m_TargetValue.Read<bool>();
				}
				break;
			}
		}

		return false;
	}

	void AnimationCondition::Serialize(SerializationNode& conditionNode)
	{
		conditionNode.WriteData("Property", m_Property->Name);
		conditionNode.WriteData("Blend Time", m_BlendTime);
		conditionNode.WriteData("Comparison", Enum::ToString(m_ComparisonOp));

		switch (m_Property->Type)
		{
			case AnimationPropertyType::Trigger:
			case AnimationPropertyType::Bool:
			{
				bool value = GetTargetValue<bool>();
				conditionNode.WriteData("Value", value);
				break;
			}
			case AnimationPropertyType::Float:
			{
				float value = GetTargetValue<float>();
				conditionNode.WriteData("Value", value);
				break;
			}
			case AnimationPropertyType::Int:
			{
				int32_t value = GetTargetValue<int32_t>();
				conditionNode.WriteData("Value", value);
				break;
			}
			default:
				break;
		}
	}

	AnimationLink::AnimationLink(Ref<AnimationState> start, Ref<AnimationState> end)
		: m_GUID(GUID::New()), m_BeginState(start), m_EndState(end)

	{
	}

	AnimationLink::AnimationLink(GUID guid, Ref<AnimationState> start, Ref<AnimationState> end)
		: m_GUID(guid), m_BeginState(start), m_EndState(end)
	{

	}

	bool AnimationLink::Evaluate(const Ref<AnimationState>& currentState, Ref<AnimationState>& nextState, float& blendTime)
	{
		if (currentState != m_EndState && m_ForwardTransition->Evaluate())
		{
			nextState = m_EndState;
			blendTime = m_ForwardTransition->GetBlendTime();
			return true;
		}

		if (currentState != m_BeginState && m_ReturnTransition->Evaluate())
		{
			nextState = m_BeginState;
			blendTime = m_ReturnTransition->GetBlendTime();
			return true;
		}

		return false;
	}

	void AnimationLink::SetTransition(Ref<AnimationState> begin, Ref<AnimationState> end, Ref<AnimationCondition>& condition)
	{
		// Check for a forward transition (begin -> end)
		if (m_BeginState == begin && m_EndState == end)
			m_ForwardTransition = condition;
		// Check for a return transition (end -> begin)
		else if (m_BeginState == end && m_EndState == begin)
			m_ReturnTransition = condition;
	}
}