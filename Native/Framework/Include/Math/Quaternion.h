#pragma once
#include "glm.h"
#include "Vector3.h"
#include "Vector4.h"
#include <sstream>
#include "Serialization.h"

namespace Odyssey
{
	using namespace glm;
	class Quaternion : public glm::quat
	{
	public:
		Quaternion();
		Quaternion(const Vector4& v);
		Quaternion(float x, float y, float z, float w);
		Quaternion(glm::quat q);
		Quaternion(const Vector3& axis, float angle);

	public:
		Quaternion operator+(const Quaternion& other) const;
		Quaternion operator-(const Quaternion& other) const;
		Quaternion operator*(const Quaternion& other) const;
		Quaternion operator*(float amt) const;
		Quaternion operator/(float amt) const;
		Quaternion operator+=(const Quaternion& other);
		Quaternion operator-=(const Quaternion& other);
		Quaternion operator*=(const Quaternion& other);
		Quaternion operator*=(float amt);
		Quaternion operator/=(float amt);
		Quaternion operator=(const Quaternion& other);
		bool operator==(const Quaternion& other) const;
		bool operator!=(const Quaternion& other) const;

	public:
		Vector3 GetRight() const;
		Vector3 GetUp() const;
		Vector3 GetForward() const;
		Vector3 GetEuler() const;
		Vector3 GetAxis() const;
		Vector3 GetRotated(const Vector3& other) const;
		Quaternion Conjugate() const;
		Quaternion Inverse() const;
		Quaternion Normalized() const;
		bool  IsNormalized() const;
		float GetAngle() const;
		float Dot(const Quaternion& other) const;
		float Length() const;
		float LengthSquared() const;
		Vector4 ToVector() const;
		std::string ToStringEuler() const;
		std::string ToString() const;

	public:
		static Quaternion FromVector(glm::vec3& rot);
		static Quaternion Euler(const Vector3& v);
		static Quaternion Euler(float x, float y, float z);
		static Quaternion AxisAngle(const Vector3& axis, float angle);
		static Quaternion Slerp(const Quaternion& from, const Quaternion& dest, float t);
		static Quaternion LookAt(const Vector3& from, const Vector3& to, const Vector3& up);

	public:
		ODYSSEY_SERIALIZE(Quaternion, x, y, z, w);
	};
}