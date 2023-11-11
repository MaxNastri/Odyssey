#include "Vector2.h"

namespace Odyssey
{
    Vector2   Vector2::Zero = Vector2(0.0f);
    Vector2   Vector2::One = Vector2(1.0f);

    Vector2::Vector2()
    {
        x = y = 0.0f;
    }

    Vector2::Vector2(float x, float y)
        : glm::vec2(x, y)
    {

    }

    Vector2::Vector2(const Vector2& rhs)
        : glm::vec2(rhs)
    {

    }

    Vector2::Vector2(float val)
        : glm::vec2(val, val)
    {

    }

    Vector2::Vector2(const glm::vec2& rhs)
        : glm::vec2(rhs.x, rhs.y)
    {

    }

    Vector2& Vector2::operator*=(Vector2 const& v)
    {
        x *= v.x;
        y *= v.y;
        return *this;
    }

    Vector2& Vector2::operator/=(Vector2 const& v)
    {
        x /= v.x;
        y /= v.y;
        return *this;
    }

    Vector2& Vector2::operator+=(Vector2 const& v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    Vector2& Vector2::operator-=(Vector2 const& v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    Vector2& Vector2::operator*=(float f)
    {
        x *= f;
        y *= f;
        return *this;
    }

    Vector2& Vector2::operator/=(float f)
    {
        x /= f;
        y /= f;
        return *this;
    }

    Vector2& Vector2::operator+=(float f)
    {
        x += f;
        y += f;
        return *this;
    }

    Vector2& Vector2::operator-=(float f)
    {
        x -= f;
        y -= f;
        return *this;
    }

    bool Vector2::operator==(const Vector2& rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }

    bool Vector2::operator!=(const Vector2& rhs) const
    {
        return !(x == rhs.x && y == rhs.y);
    }

    bool Vector2::operator>(const Vector2& rhs) const
    {
        return length() > rhs.length();
    }

    bool Vector2::operator<(const Vector2& rhs) const
    {
        return length() < rhs.length();
    }

    float& Vector2::operator[](unsigned int i)
    {
        return (&x)[i];
    }

    Vector2 Vector2::operator-() const
    {
        return Vector2(-1.0f * x, -1.0f * y);
    }

    std::ostream& Vector2::operator<<(std::ostream& os)
    {
        return os << "X:" << x << "   Y:" << y;
    }

    Vector2 Vector2::Abs() const
    {
        return Vector2(glm::abs(x), glm::abs(y));
    }

    Vector2 Vector2::Min(const Vector2& other) const
    {
        return length() < other.length() ? glm::vec2(*this) : glm::vec2(other);
    }

    Vector2 Vector2::Max(const Vector2& other) const
    {
        return length() > other.length() ? glm::vec2(*this) : glm::vec2(other);
    }

    Vector2 Vector2::Normalized() const
    {
        glm::vec2 norm = glm::normalize(glm::vec2(*this));
        return norm;
    }

    Vector2 Vector2::Project(const Vector2& normal) const
    {
        return glm::proj(glm::vec2(*this), glm::vec2(normal));
    }

    Vector2 Vector2::Rotate(const Vector2& axis, float angle) const
    {
        return glm::rotate(glm::vec2(axis), angle);
    }

    Vector2 Vector2::Reflect(const Vector2& normal) const
    {
        return glm::reflect(glm::vec2(*this), glm::vec2(normal));
    }

    Vector2 Vector2::Refract(const Vector2& normal, float indexOfRefraction) const
    {
        return glm::refract(glm::vec2(*this), glm::vec2(normal), indexOfRefraction);
    }

    float Vector2::Dot(const Vector2& other) const
    {
        return glm::dot<2, float, glm::qualifier::highp>(glm::vec2(*this), glm::vec2(other));
    }

    float Vector2::Distance(const Vector2& other) const
    {
        return glm::distance(glm::vec2(*this), glm::vec2(other));
    }

    float Vector2::Magnitude() const
    {
        return glm::length(glm::vec2(*this));
    }

    float Vector2::MagnitudeSqrt() const
    {
        return glm::length2(glm::vec2(*this));
    }

    float Vector2::Max() const
    {
        return glm::max(x, y);
    }

    float Vector2::Min() const
    {
        return glm::min(x, y);
    }

    float Vector2::Avg() const
    {
        return (x + y) / 2.0f;
    }

    void Vector2::Normalize()
    {
        *this = glm::normalize(glm::vec2(*this));
    }

    float* Vector2::Get()
    {
        return &x;
    }

    std::string Vector2::ToString() const
    {
        std::stringstream ss;
        ss << "X:" << x << "   Y:" << y;
        return ss.str();
    }
}
