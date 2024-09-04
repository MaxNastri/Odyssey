#pragma once

namespace Odyssey
{
	struct BlendKey
	{
		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;
	};

	class BoneKeyframe
	{
	public:
		template<typename T> struct Key { double Time; T Value; };
		typedef Key<glm::vec3> PositionKey;
		typedef Key<glm::quat> RotationKey;
		typedef Key<glm::vec3> ScaleKey;

	public:
		BoneKeyframe() = default;
	public:
		void AddPositionKey(double time, glm::vec3 value) { m_PositionKeys.push_back({ time, value }); }
		void AddRotationKey(double time, glm::quat value) { m_RotationKeys.push_back({ time, value }); }
		void AddScaleKey(double time, glm::vec3 value) { m_ScaleKeys.push_back({ time, value }); }
		void SetBoneName(std::string_view boneName) { m_Name = boneName; }

	public:
		const std::vector<PositionKey>& GetPositionKeys() { return m_PositionKeys; }
		const std::vector<RotationKey>& GetRotationKeys() { return m_RotationKeys; }
		const std::vector<ScaleKey>& GetScaleKeys() { return m_ScaleKeys; }
		const double GetFrameTime(size_t frameIndex) { return m_PositionKeys[frameIndex].Time; }

		std::string_view GetName() { return m_Name; }

	public:
		glm::mat4 GetKey(double time, bool loop, double duration)
		{
			glm::vec3 position;
			glm::quat rotation;
			glm::vec3 scale;

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
			glm::mat4 t = glm::translate(glm::identity<mat4>(), position);
			glm::mat4 r = glm::mat4_cast(rotation);
			glm::mat4 s = glm::scale(glm::identity<mat4>(), scale);

			// Return TRS
			return t * r * s;
		}

		glm::mat4 BlendKeysOld(size_t prevKey, size_t nextKey, float blendFactor)
		{
			glm::vec3 position;
			glm::quat rotation;
			glm::vec3 scale = glm::vec3(1,1,1);

			position = glm::mix(m_PositionKeys[prevKey].Value, m_PositionKeys[nextKey].Value, blendFactor);
			rotation = glm::slerp(m_RotationKeys[prevKey].Value, m_RotationKeys[nextKey].Value, blendFactor);
			scale = glm::mix(m_ScaleKeys[prevKey].Value, m_ScaleKeys[nextKey].Value, blendFactor);

			// Convert to matrices
			glm::mat4 t = glm::translate(glm::identity<mat4>(), position);
			glm::mat4 r = glm::mat4_cast(rotation);
			glm::mat4 s = glm::scale(glm::identity<mat4>(), scale);

			// Return TRS
			return t * r * s;
		}

		BlendKey BlendKeys(size_t prevKey, size_t nextKey, float blendFactor)
		{
			BlendKey blendKey;
			blendKey.position = glm::mix(m_PositionKeys[prevKey].Value, m_PositionKeys[nextKey].Value, blendFactor);
			blendKey.rotation = glm::slerp(m_RotationKeys[prevKey].Value, m_RotationKeys[nextKey].Value, blendFactor);
			blendKey.scale = glm::mix(m_ScaleKeys[prevKey].Value, m_ScaleKeys[nextKey].Value, blendFactor);
			return blendKey;
		}
	private:
		template<typename KeyType>
		void FindKeys(double time, std::vector<KeyType>& keys, KeyType& outPrev, KeyType& outNext, bool loop)
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

	private:
		std::string m_Name;
		std::vector<PositionKey> m_PositionKeys;
		std::vector<RotationKey> m_RotationKeys;
		std::vector<ScaleKey> m_ScaleKeys;
	};
}