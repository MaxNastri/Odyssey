#include "BoneKeyframe.hpp"

namespace Odyssey
{
	static constexpr double s_Epsilon = 0.000001;


	// time = 0.3
	// array = {0.0, 0.6}
	void BoneKeyframe::AddPositionKey(double time, glm::vec3 value)
	{
		for (size_t i = 0; i < m_PositionKeys.size(); i++)
		{
			if (m_PositionKeys[i].Time > time)
			{
				m_PositionKeys.insert(m_PositionKeys.begin() + i, PositionKey(time, value));
				return;
			}
		}

		m_PositionKeys.push_back(PositionKey(time, value));
	}

	void BoneKeyframe::AddRotationKey(double time, glm::quat value)
	{
		for (size_t i = 0; i < m_RotationKeys.size(); i++)
		{
			if (m_RotationKeys[i].Time > time)
			{
				m_RotationKeys.insert(m_RotationKeys.begin() + i, RotationKey(time, value));
				return;
			}
		}

		m_RotationKeys.push_back(RotationKey(time, value));
	}

	void BoneKeyframe::AddScaleKey(double time, glm::vec3 value)
	{
		for (size_t i = 0; i < m_ScaleKeys.size(); i++)
		{
			if (m_ScaleKeys[i].Time > time)
			{
				m_ScaleKeys.insert(m_ScaleKeys.begin() + i, ScaleKey(time, value));
				return;
			}
		}

		m_ScaleKeys.push_back(ScaleKey(time, value));
	}

	bool BoneKeyframe::HasPositionKey(double time)
	{
		for (size_t i = 0; i < m_PositionKeys.size(); i++)
		{
			if (std::abs(m_PositionKeys[i].Time - time) < s_Epsilon)
				return true;
		}

		return false;
	}

	bool BoneKeyframe::HasRotationKey(double time)
	{
		for (size_t i = 0; i < m_RotationKeys.size(); i++)
		{
			if (std::abs(m_RotationKeys[i].Time - time) < s_Epsilon)
				return true;
		}

		return false;
	}

	bool BoneKeyframe::HasScaleKey(double time)
	{
		for (size_t i = 0; i < m_ScaleKeys.size(); i++)
		{
			if (std::abs(m_ScaleKeys[i].Time - time) < s_Epsilon)
				return true;
		}

		return false;
	}

	void BoneKeyframe::SortKeys()
	{
		for (size_t i = m_PositionKeys.size() - 1; i >= 1;)
		{
			PositionKey current = m_PositionKeys[i];
			PositionKey next = m_PositionKeys[i - 1];

			if (current.Time < next.Time)
				std::swap(m_PositionKeys[i], m_PositionKeys[i - 1]);
			else
				i--;
		}
	}

}