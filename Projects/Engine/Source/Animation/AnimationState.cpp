#include "AnimationState.h"
#include "AnimationNodes.h"

namespace Odyssey
{
	AnimationLink::AnimationLink(AnimationState* start, AnimationState* end, std::shared_ptr<AnimationProperty> property, ComparisonOp compareOp, RawBuffer targetValue)
		: m_Start(start), m_End(end),
		m_Property(property), m_CompareOp(compareOp)

	{
		 RawBuffer::Copy(m_TargetValue, targetValue);
	}

	bool AnimationLink::Evaluate()
	{
		switch (m_CompareOp)
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

	AnimationState::AnimationState(GUID animationClip)
	{
		m_AnimationClip = animationClip;
	}

	AnimationState* AnimationState::Evaluate()
	{
		for (auto& link : m_Links)
		{
			if (link->Evaluate())
				return link->GetEndState();
		}

		return nullptr;
	}

	std::string_view AnimationState::GetName()
	{
		return "";
	}

	GUID AnimationState::GetClip()
	{
		return m_AnimationClip;
	}

	std::shared_ptr<AnimationLink> AnimationState::AddLink(AnimationState* connectedState, std::shared_ptr<AnimationProperty> property, ComparisonOp compareOp, RawBuffer targetValue)
	{
		auto& link = m_Links.emplace_back(std::make_shared<AnimationLink>(this, connectedState, property, compareOp, targetValue));


		return link;
	}
}