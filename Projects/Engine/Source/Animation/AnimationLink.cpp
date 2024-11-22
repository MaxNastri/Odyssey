#include "AnimationLink.h"
#include "AnimationState.h"
#include "AnimationProperty.hpp"

namespace Odyssey
{
	AnimationLink::AnimationLink(std::shared_ptr<AnimationState> start, std::shared_ptr<AnimationState> end, std::shared_ptr<AnimationProperty> property, ComparisonOp compareOp, RawBuffer& targetValue)
		: m_GUID(GUID::New()), m_BeginState(start), m_EndState(end),
		m_Property(property), m_ComparisonOp(compareOp)

	{
		RawBuffer::Copy(m_TargetValue, targetValue);
	}

	AnimationLink::AnimationLink(GUID guid, std::shared_ptr<AnimationState> start, std::shared_ptr<AnimationState> end, std::shared_ptr<AnimationProperty> property, ComparisonOp compareOp, RawBuffer& targetValue)
		: m_GUID(guid), m_BeginState(start), m_EndState(end),
		m_Property(property), m_ComparisonOp(compareOp)
	{
		RawBuffer::Copy(m_TargetValue, targetValue);
	}

	bool AnimationLink::Evaluate()
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

	void AnimationLink::SetFloat(float value)
	{
		static constexpr size_t floatSize = sizeof(float);
		if (m_TargetValue.GetSize() != floatSize)
		{
			m_TargetValue.Free();
			m_TargetValue.Allocate(floatSize);
		}

		m_TargetValue.Write(&value);
	}

	void AnimationLink::SetInt(int32_t value)
	{
		static constexpr size_t intSize = sizeof(int32_t);
		if (m_TargetValue.GetSize() != intSize)
		{
			m_TargetValue.Free();
			m_TargetValue.Allocate(intSize);
		}

		m_TargetValue.Write(&value);
	}

	void AnimationLink::SetBool(bool value)
	{
		static constexpr size_t boolSize = sizeof(bool);
		if (m_TargetValue.GetSize() != boolSize)
		{
			m_TargetValue.Free();
			m_TargetValue.Allocate(boolSize);
		}

		m_TargetValue.Write(&value);
	}

	void AnimationLink::SetTrigger(bool value)
	{
		static constexpr size_t boolSize = sizeof(bool);
		if (m_TargetValue.GetSize() != boolSize)
		{
			m_TargetValue.Free();
			m_TargetValue.Allocate(boolSize);
		}

		m_TargetValue.Write(&value);
	}
}