#include "Vector2.h"

namespace Odyssey
{
    Vector2   Vector2::Zero = Vector2(0.0f);
    Vector2   Vector2::One = Vector2(1.0f);

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

    std::string Vector2::ToString() const
    {
        std::stringstream ss;
        ss << "X:" << x << "   Y:" << y;
        return ss.str();
    }
}
