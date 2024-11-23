#include "BoneKeyframe.h"

namespace Odyssey
{
	static constexpr double s_Epsilon = 0.000001;

	template<typename KeyType>
	inline static void FindKeys(double time, std::vector<KeyType>& keys, KeyType& outPrev, KeyType& outNext, bool loop)
	{
		for (size_t i = 0; i < keys.size(); i++)
		{
			// Look for the first key frame past our time
			if (keys[i].Time >= time)
			{
				// That's our next key
				outNext = keys[i];

				// Use the previous key directly
				if (i > 0)
				{
					outPrev = keys[i - 1];
				}
				// Rare case where key 0 is our next key
				else
				{
					if (loop)
						// Loop back to the end of the animation as our previous
						outPrev = keys[keys.size() - 1];
					else
						// Don't loop, so we don't blend the animation
						outPrev = keys[i];
				}

				// Break out
				break;
			}
		}
	}

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

	mat4 BoneKeyframe::GetKey(double time, bool loop, double duration)
	{
		float3 position;
		quat rotation;
		float3 scale;

		if (time == 0)
		{
			position = m_PositionKeys[0].Value;
			rotation = m_RotationKeys[0].Value;
			scale = m_ScaleKeys[0].Value;
		}
		else
		{
			// Find the position keys and lerp them
			{
				PositionKey prev, next;
				FindKeys<PositionKey>(time, m_PositionKeys, prev, next, loop);

				double totalTime = next.Time == 0.0 ? duration : next.Time;

				if (prev.Time == next.Time)
					position = prev.Value;
				else
				{
					double ratio = (time - prev.Time) / (totalTime - prev.Time);
					position = glm::mix(prev.Value, next.Value, ratio);
				}
			}

			// Find the rotation keys and lerp them
			{
				RotationKey prev, next;
				FindKeys<RotationKey>(time, m_RotationKeys, prev, next, loop);

				double totalTime = next.Time == 0.0 ? duration : next.Time;
				// Same frame, don't blend
				if (prev.Time == next.Time)
					rotation = prev.Value;
				else
				{
					double ratio = (time - prev.Time) / (next.Time - prev.Time);
					rotation = glm::slerp(prev.Value, next.Value, (float)ratio);
				}
			}

			// Find the scale keys and lerp them
			{
				ScaleKey prev, next;
				FindKeys<ScaleKey>(time, m_ScaleKeys, prev, next, loop);

				double totalTime = next.Time == 0.0 ? duration : next.Time;
				// Same frame, don't blend
				if (prev.Time == next.Time)
					scale = prev.Value;
				else
				{
					double ratio = (time - prev.Time) / (next.Time - prev.Time);
					scale = glm::mix(prev.Value, next.Value, ratio);
				}
			}
		}

		// Convert to matrices
		mat4 t = glm::translate(glm::identity<mat4>(), position);
		mat4 r = glm::mat4_cast(rotation);
		mat4 s = glm::scale(glm::identity<mat4>(), scale);

		// Return TRS
		return t * r * s;
	}

	BlendKey BoneKeyframe::BlendKeys(size_t prevKey, size_t nextKey, float blendFactor)
	{
		BlendKey blendKey;
		blendKey.Position = glm::mix(m_PositionKeys[prevKey].Value, m_PositionKeys[nextKey].Value, blendFactor);
		blendKey.Rotation = glm::slerp(m_RotationKeys[prevKey].Value, m_RotationKeys[nextKey].Value, blendFactor);
		blendKey.Scale = glm::mix(m_ScaleKeys[prevKey].Value, m_ScaleKeys[nextKey].Value, blendFactor);
		return blendKey;
	}

}