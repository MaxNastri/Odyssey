#pragma once
#include "glm.h"
#include "Vector3.h"
#include "Vector2.h"
#include <sstream>
#include <Serialization.h>

namespace Odyssey
{
	class Vector4 : public glm::vec4
	{
	public:
		Vector4();
		Vector4(float x, float y, float z, float w);
		Vector4(const Vector4& rhs);
		Vector4(const Vector3& rhs);
		Vector4(const Vector2& rhs);
		Vector4(float f);
		Vector4(const glm::vec4& rhs);
		Vector4(const Vector3& src, float w);

	public:
		Vector4& operator*=(Vector4 const& v);
		Vector4& operator/=(Vector4 const& v);
		Vector4& operator+=(Vector4 const& v);
		Vector4& operator-=(Vector4 const& v);
		Vector4& operator*=(float f);
		Vector4& operator/=(float f);
		Vector4& operator+=(float f);
		Vector4& operator-=(float f);
		bool operator==(const Vector4& rhs) const;
		bool operator!=(const Vector4& rhs) const;
		bool operator>(const Vector4& rhs) const;
		bool operator<(const Vector4& rhs) const;
		float& operator[](unsigned int i);
		Vector4 operator-() const;
		std::ostream& operator<<(std::ostream& os);

	public:
		Vector4 Abs() const;
		Vector4 Min(const Vector4& other) const;
		Vector4 Max(const Vector4& other) const;
		Vector4 Normalized() const;
		Vector4 Project(const Vector4& normal) const;
		Vector4 Rotate(const class Quaternion& rotation) const;
		Vector4 Rotate(const Vector3& axis, float angle) const;
		Vector4 Reflect(const Vector4& normal) const;
		Vector4 Refract(const Vector4& normal, float indexOfRefraction) const;
		Vector3 XYZ();
		Vector2 XY();
		float Dot(const Vector4& other) const;
		float Distance(const Vector4& other) const;
		float Magnitude() const;
		float MagnitudeSqrt() const;
		float Max() const;
		float Min() const;
		float Avg() const;
		void  Normalize();
		float* Get();
		std::string ToString() const;

	public:
		static Vector4 Zero;
		static Vector4 One;
		ODYSSEY_SERIALIZE(Vector4, x, y, z, w)
	};
}