#pragma once
#include "GUID.h"
#include "RawBuffer.h"
#include "Ref.h"
#include "AnimationState.h"
#include "AnimationProperty.h"

namespace Odyssey
{
	inline static const std::array<std::string, 5> Comparison_Op_Display =
	{
		"<", "<=", "=", ">", ">="
	};

	enum class ComparisonOp
	{
		Less = 0,
		LessOrEqual = 1,
		Equal = 2,
		Greater = 3,
		GreaterOrEqual = 4,
	};

	class AnimationCondition
	{
	public:
		AnimationCondition() = default;
		AnimationCondition(Ref<AnimationProperty> property, ComparisonOp comparison, const RawBuffer& targetValue, float blendTime = 1.0f);

	public:
		bool Evaluate();

	public:
		void Serialize(SerializationNode& conditionNode);

	public:
		const std::string& GetPropertyName() { return m_Property->Name; }
		AnimationPropertyType GetPropertyType() { return m_Property->Type; }
		ComparisonOp GetComparison() { return m_ComparisonOp; }

	public:
		float GetBlendTime() { return m_BlendTime; }
		void SetBlendTime(float blendTime) { m_BlendTime = blendTime; }

	public:
		template<typename T>
		T GetTargetValue() { return m_TargetValue.Read<T>(); }

		template<typename T>
		void SetTargetValue(T value) { m_TargetValue.Write(&value); }

	private:
		Ref<AnimationProperty> m_Property;
		ComparisonOp m_ComparisonOp;
		RawBuffer m_TargetValue;
		float m_BlendTime = 1.0f;
	};

	class AnimationLink
	{
	public:
		AnimationLink(Ref<AnimationState> start, Ref<AnimationState> end);
		AnimationLink(GUID guid, Ref<AnimationState> start, Ref<AnimationState> end);

	public:
		bool Evaluate(const Ref<AnimationState>& currentState, Ref<AnimationState>& nextState, float& blendTime);

	public:
		void SetTransition(Ref<AnimationState> begin, Ref<AnimationState> end, Ref<AnimationCondition>& condition);

	public:
		GUID GetGUID() { return m_GUID; }
		Ref<AnimationState> GetBeginState() { return m_BeginState; }
		Ref<AnimationState> GetEndState() { return m_EndState; }
		Ref<AnimationCondition>& GetForwardTransition() { return m_ForwardTransition; }
		Ref<AnimationCondition>& GetReturnTransition() { return m_ReturnTransition; }
	
	private:
		GUID m_GUID;
		Ref<AnimationState> m_BeginState;
		Ref<AnimationState> m_EndState;

		Ref<AnimationCondition> m_ForwardTransition;
		Ref<AnimationCondition> m_ReturnTransition;
	};
}