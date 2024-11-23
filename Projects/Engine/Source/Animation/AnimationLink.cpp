#include "AnimationLink.h"
#include "AnimationState.h"

namespace Odyssey
{
	AnimationCondition::AnimationCondition(Ref<AnimationProperty> property, ComparisonOp comparison, const RawBuffer& targetValue)
	{
		m_Property = property;
		m_ComparisonOp = comparison;
		RawBuffer::Copy(m_TargetValue, targetValue);
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

	AnimationLink::AnimationLink(Ref<AnimationState> start, Ref<AnimationState> end)
		: m_GUID(GUID::New()), m_BeginState(start), m_EndState(end)

	{
	}

	AnimationLink::AnimationLink(GUID guid, Ref<AnimationState> start, Ref<AnimationState> end)
		: m_GUID(guid), m_BeginState(start), m_EndState(end)
	{

	}

	bool AnimationLink::Evaluate(Ref<AnimationState>& currentState)
	{
		for (Ref<AnimationCondition> condition : m_ForwardTransitions)
		{
			if (condition->Evaluate())
			{
				currentState = m_EndState;
				return true;
			}
		}

		for (Ref<AnimationCondition> condition : m_ReturnTransitions)
		{
			if (condition->Evaluate())
			{
				currentState = m_BeginState;
				return true;
			}
		}

		return false;
	}
	void AnimationLink::AddTransition(Ref<AnimationState> begin, Ref<AnimationState> end, Ref<AnimationCondition>& condition)
	{
		// Check for a forward transition (begin -> end)
		if (m_BeginState == begin && m_EndState == end)
			m_ForwardTransitions.emplace_back(condition);
		// Check for a return transition (end -> begin)
		else if (m_BeginState == end && m_EndState == begin)
			m_ReturnTransitions.emplace_back(condition);
	}
}