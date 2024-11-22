#pragma once

namespace Odyssey
{
	struct BlendKey
	{
		vec3 Position;
		quat Rotation;
		vec3 Scale;
	};

	class BoneKeyframe
	{
	public:
		template<typename T> struct Key { double Time; T Value; };
		typedef Key<vec3> PositionKey;
		typedef Key<quat> RotationKey;
		typedef Key<vec3> ScaleKey;

	public:
		BoneKeyframe() = default;

	public:
		void AddPositionKey(double time, glm::vec3 value);
		void AddRotationKey(double time, glm::quat value);
		void AddScaleKey(double time, glm::vec3 value);
		bool HasPositionKey(double time);
		bool HasRotationKey(double time);
		bool HasScaleKey(double time);
		void SetBoneName(std::string_view boneName) { m_Name = boneName; }

	public:
		void SortKeys();

	public:
		const std::vector<PositionKey>& GetPositionKeys() { return m_PositionKeys; }
		const std::vector<RotationKey>& GetRotationKeys() { return m_RotationKeys; }
		const std::vector<ScaleKey>& GetScaleKeys() { return m_ScaleKeys; }
		const double GetFrameTime(size_t frameIndex) { return m_PositionKeys[frameIndex].Time; }
		std::string_view GetName() { return m_Name; }

	public:
		mat4 GetKey(double time, bool loop, double duration);
		BlendKey BlendKeys(size_t prevKey, size_t nextKey, float blendFactor);

	private:
		std::string m_Name;
		std::vector<PositionKey> m_PositionKeys;
		std::vector<RotationKey> m_RotationKeys;
		std::vector<ScaleKey> m_ScaleKeys;
	};
}