#pragma once
#include "GUID.h"
#include "RawBuffer.hpp"

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

	class AnimationState;
	struct AnimationProperty;

	class AnimationLink
	{
	public:
		AnimationLink(std::shared_ptr<AnimationState> start, std::shared_ptr<AnimationState> end, std::shared_ptr<AnimationProperty> property, ComparisonOp compareOp, RawBuffer& targetValue);
		AnimationLink(GUID guid, std::shared_ptr<AnimationState> start, std::shared_ptr<AnimationState> end, std::shared_ptr<AnimationProperty> property, ComparisonOp compareOp, RawBuffer& targetValue);

	public:
		bool Evaluate();

	public:
		GUID GetGUID() { return m_GUID; }
		std::shared_ptr<AnimationState> GetBeginState() { return m_BeginState; }
		std::shared_ptr<AnimationState> GetEndState() { return m_EndState; }
		std::shared_ptr<AnimationProperty> GetProperty() { return m_Property; }
		ComparisonOp GetComparisonOp() { return m_ComparisonOp; }
		RawBuffer& GetTargetValue() { return m_TargetValue; }

	public:
		void SetProperty(std::shared_ptr<AnimationProperty> property) { m_Property = property; }
		void SetComparisonOp(ComparisonOp op) { m_ComparisonOp = op; }
		void SetFloat(float value);
		void SetInt(int32_t value);
		void SetBool(bool value);
		void SetTrigger(bool value);

	private:
		GUID m_GUID;
		std::shared_ptr<AnimationState> m_BeginState;
		std::shared_ptr<AnimationState> m_EndState;

		std::shared_ptr<AnimationProperty> m_Property;
		ComparisonOp m_ComparisonOp;
		RawBuffer m_TargetValue;
	};
}