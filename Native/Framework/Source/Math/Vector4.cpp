#include "Vector4.h"
#include "Quaternion.h"

namespace Odyssey
{
    Vector4   Vector4::Zero = Vector4(0.0f);
    Vector4   Vector4::One = Vector4(1.0f);

    Vector4::Vector4()
    {
        x = y = z = w = 0.0f;
    }

    Vector4::Vector4(float x, float y, float z, float w)
        : glm::vec4(x, y, z, w)
    {

    }

    Vector4::Vector4(const Vector4& rhs)
        : glm::vec4(rhs)
    {

    }

    Vector4::Vector4(const Vector3& rhs)
        : glm::vec4(rhs.x, rhs.y, rhs.z, 0.0f)
    {

    }

    Vector4::Vector4(const Vector2& rhs)
        : glm::vec4(rhs.x, rhs.y, 0.0f, 0.0f)
    {

    }

    Vector4::Vector4(float f)
        : glm::vec4(f, f, f, f)
    {

    }

    Vector4::Vector4(const glm::vec4& rhs)
        : glm::vec4(rhs.x, rhs.y, rhs.z, rhs.w)
    {

    }

    Vector4::Vector4(const Vector3& src, float w)
        : glm::vec4(src, w)
    {

    }

    Vector4& Vector4::operator*=(Vector4 const& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        w *= v.w;
        return *this;
    }

    Vector4& Vector4::operator/=(Vector4 const& v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        w /= v.w;
        return *this;
    }

    Vector4& Vector4::operator+=(Vector4 const& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;
        return *this;
    }

    Vector4& Vector4::operator-=(Vector4 const& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;
        return *this;
    }

    Vector4& Vector4::operator*=(float f)
    {
        x *= f;
        y *= f;
        z *= f;
        w *= f;
        return *this;
    }

    Vector4& Vector4::operator/=(float f)
    {
        x /= f;
        y /= f;
        z /= f;
        w /= f;
        return *this;
    }

    Vector4& Vector4::operator+=(float f)
    {
        x += f;
        y += f;
        z += f;
        w += f;
        return *this;
    }

    Vector4& Vector4::operator-=(float f)
    {
        x -= f;
        y -= f;
        z -= f;
        w -= f;
        return *this;
    }

    bool Vector4::operator==(const Vector4& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }

    bool Vector4::operator!=(const Vector4& rhs) const
    {
        return !(x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
    }

    bool Vector4::operator>(const Vector4& rhs) const
    {
        return length() > rhs.length();
    }

    bool Vector4::operator<(const Vector4& rhs) const
    {
        return length() < rhs.length();
    }

    float& Vector4::operator[](unsigned int i)
    {
        return (&x)[i];
    }

    Vector4 Vector4::operator-() const
    {
        return Vector4(-1.0f * x, -1.0f * y, -1.0f * z, -1.0f * w);
    }

    std::ostream& Vector4::operator<<(std::ostream& os)
    {
        return os << "X:" << x << "   Y:" << y << "   Z:" << z << "   W:" << w;
    }

    Vector4 Vector4::Abs() const
    {
        return Vector4(glm::abs(x), glm::abs(y), glm::abs(z), glm::abs(w));
    }

    Vector4 Vector4::Min(const Vector4& other) const
    {
        return length() < other.length() ? *this : other;
    }

    Vector4 Vector4::Max(const Vector4& other) const
    {
        return length() > other.length() ? *this : other;
    }

    Vector4 Vector4::Normalized() const
    {
        glm::vec4 norm = glm::normalize(glm::vec4(*this));
        return norm;
    }

    Vector4 Vector4::Project(const Vector4& normal) const
    {
        return glm::proj(glm::vec4(*this), glm::vec4(normal));
    }

    Vector4 Vector4::Rotate(const Quaternion& rotation) const
    {
        return glm::rotate(glm::quat(rotation), glm::vec4(*this));
    }

    Vector4 Vector4::Rotate(const Vector3& axis, float angle) const
    {
        return glm::rotate(glm::vec4(*this), angle, glm::vec3(axis));
    }

    Vector4 Vector4::Reflect(const Vector4& normal) const
    {
        return glm::reflect(glm::vec4(*this), glm::vec4(normal));
    }

    Vector4 Vector4::Refract(const Vector4& normal, float indexOfRefraction) const
    {
        return glm::refract(glm::vec4(*this), glm::vec4(normal), indexOfRefraction);
    }

    Vector3 Vector4::XYZ()
    {
        return Vector3(x, y, z);
    }

    Vector2 Vector4::XY()
    {
        return Vector2(x, y);
    }

    float Vector4::Dot(const Vector4& other) const
    {
        return glm::dot<4, float, glm::qualifier::highp>(glm::vec4(*this), glm::vec4(other));
    }

    float Vector4::Distance(const Vector4& other) const
    {
        return glm::distance(glm::vec4(*this), glm::vec4(other));
    }

    float Vector4::Magnitude() const
    {
        return glm::length(glm::vec4(*this));
    }

    float Vector4::MagnitudeSqrt() const
    {
        return glm::length2(glm::vec4(*this));
    }

    float Vector4::Max() const
    {
        return glm::max(x, glm::max(y, glm::max(z, w)));
    }

    float Vector4::Min() const
    {
        return glm::min(x, glm::min(y, glm::min(z, w)));
    }

    float Vector4::Avg() const
    {
        return (x + y + z + w) / 4.0f;
    }

    void Vector4::Normalize()
    {
        *this = glm::normalize(*this);
    }

    float* Vector4::Get()
    {
        return &x;
    }

    std::string Vector4::ToString() const
    {
        std::stringstream ss;
        ss << "X:" << x << "   Y:" << y << "   Z:" << z << "   W:" << w;
        return ss.str();
    }
}