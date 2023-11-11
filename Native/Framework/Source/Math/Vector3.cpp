#include "Vector3.h"
#include "Quaternion.h"

namespace Odyssey
{
	Vector3 Vector3::Zero = Vector3(0.0f);
	Vector3 Vector3::One = Vector3(1.0f);
	Vector3 Vector3::Forward = Vector3(0.0f, 0.0f, 1.0f);
	Vector3 Vector3::Up = Vector3(0.0f, 1.0f, 0.0f);
	Vector3 Vector3::Right = Vector3(1.0f, 0.0f, 0.0f);
	Vector3 Vector3::Back = Vector3(0.0f, 0.0f, -1.0f);
	Vector3 Vector3::Down = Vector3(0.0f, -1.0f, 0.0f);

	Vector3::Vector3(float x, float y, float z)
		: glm::vec3(x, y, z)
	{

	}

	Vector3::Vector3(const Vector3& rhs)
		: glm::vec3(rhs)
	{

	}

	Vector3::Vector3(const Vector2& rhs)
		: glm::vec3(rhs.x, rhs.y, 0.0f)
	{

	}

	Vector3::Vector3(float val)
		: glm::vec3(val, val, val)
	{

	}

	Vector3::Vector3(const glm::vec3& rhs)
		: glm::vec3(rhs.x, rhs.y, rhs.z)
	{

	}

	Vector3& Vector3::operator*=(Vector3 const& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	Vector3& Vector3::operator/=(Vector3 const& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	Vector3& Vector3::operator+=(Vector3 const& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector3& Vector3::operator-=(Vector3 const& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	Vector3& Vector3::operator*=(float f)
	{
		x *= f;
		y *= f;
		z *= f;
		return *this;
	}

	Vector3& Vector3::operator/=(float f)
	{
		x /= f;
		y /= f;
		z /= f;
		return *this;
	}

	Vector3& Vector3::operator+=(float f)
	{
		x += f;
		y += f;
		z += f;
		return *this;
	}

	Vector3& Vector3::operator-=(float f)
	{
		x -= f;
		y -= f;
		z -= f;
		return *this;
	}

	bool Vector3::operator==(const Vector3& rhs) const
	{
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}

	bool Vector3::operator!=(const Vector3& rhs) const
	{
		return !(x == rhs.x && y == rhs.y && z == rhs.z);
	}

	bool Vector3::operator>(const Vector3& rhs) const
	{
		return length() > rhs.length();
	}

	bool Vector3::operator<(const Vector3& rhs) const
	{
		return length() < rhs.length();
	}

	float& Vector3::operator[](unsigned int i)
	{
		return (&x)[i];
	}

	Vector3 Vector3::operator-() const
	{
		return Vector3(-x, -y, -z);
	}

	std::ostream& Vector3::operator<<(std::ostream& os)
	{
		return os << "X:" << x << "   Y:" << y << "   Z:" << z;
	}

	Vector3 Vector3::Lerp(const Vector3& from, const Vector3& to, float t)
	{
		return Vector3(from.x + (to.x - from.x) * t, from.y + (to.y - from.y) * t, from.z + (to.z - from.z) * t);
	}

	Vector3 Vector3::Cross(const Vector3& other) const
	{
		return glm::cross(glm::vec3(*this), glm::vec3(other));
	}

	Vector3 Vector3::Abs() const
	{
		return glm::vec3(glm::abs(x), glm::abs(y), glm::abs(z));
	}

	Vector3 Vector3::Min(const Vector3& other) const
	{
		return length() < other.length() ? glm::vec3(*this) : glm::vec3(other);
	}

	Vector3 Vector3::Max(const Vector3& other) const
	{
		return length() > other.length() ? glm::vec3(*this) : glm::vec3(other);
	}

	Vector3 Vector3::Normalized() const
	{
		glm::vec3 norm = glm::normalize(*this);
		return norm;
	}

	Vector3 Vector3::Project(const Vector3& normal) const
	{
		return glm::proj(glm::vec3(*this), glm::vec3(normal));
	}

	Vector3 Vector3::Rotate(const Vector3& axis, float angle) const
	{
		return glm::rotate(glm::vec3(*this), angle, glm::vec3(axis));
	}

	Vector3 Vector3::Rotate(const Quaternion& rotation) const
	{
		return glm::rotate(glm::quat(rotation), glm::vec3(*this));
	}

	Vector3 Vector3::Reflect(const Vector3& normal) const
	{
		return glm::reflect(glm::vec3(*this), glm::vec3(normal));
	}

	Vector3 Vector3::Refract(const Vector3& normal, float indexOfRefraction) const
	{
		return glm::refract(glm::vec3(*this), glm::vec3(normal), indexOfRefraction);
	}

	Vector2 Vector3::XY()
	{
		return Vector2(x, y);
	}

	float Vector3::Dot(const Vector3& other) const
	{
		return glm::dot<3, float, glm::qualifier::highp>(glm::vec3(*this), glm::vec3(other));
	}

	float Vector3::Distance(const Vector3& other) const
	{
		return glm::distance(glm::vec3(*this), glm::vec3(other));
	}

	float Vector3::Magnitude() const
	{
		return glm::length(glm::vec3(*this));
	}

	float Vector3::SqrMagnitude() const
	{
		return glm::length2(glm::vec3(*this));
	}

	float Vector3::Max() const
	{
		return glm::max(x, glm::max(y, z));
	}

	float Vector3::Min() const
	{
		return glm::min(x, glm::min(y, z));
	}

	float Vector3::Avg() const
	{
		return (x + y + z) / 3.0f;
	}

	void Vector3::Normalize()
	{
		*this = glm::normalize(*this);
	}

	float* Vector3::Get()
	{
		return &x;
	}

	std::string Vector3::ToString() const
	{
		std::stringstream ss;
		ss << "X:" << x << "   Y:" << y << "   Z:" << z;
		return ss.str();
	}
}