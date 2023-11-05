#pragma once
#include "glm.h"
#include <sstream>

namespace Odyssey
{
    using namespace glm;
    class Vector2 : public glm::vec2
    {
    public:
        Vector2() = default;
        Vector2(float x, float y)
            : glm::vec2(x, y) {};
        Vector2(const Vector2 & rhs)
            : glm::vec2(rhs) {};
        Vector2(float val)
            : glm::vec2(val, val) {};
        Vector2(const glm::vec2 & rhs)
            : glm::vec2(rhs.x, rhs.y) {};

        static Vector2 Zero;
        static Vector2 One;

        Vector2 Abs() const;
        Vector2 Min(const Vector2& other) const;
        Vector2 Max(const Vector2& other) const;
        Vector2 Normalized() const;
        Vector2 Project(const Vector2& normal) const;
        Vector2 Rotate(const Vector2& axis, float angle) const;
        Vector2 Reflect(const Vector2& normal) const;
        Vector2 Refract(const Vector2& normal, float indexOfRefraction) const;
        float   Dot(const Vector2& other) const;
        float   Distance(const Vector2& other) const;
        float   Magnitude() const;
        float   MagnitudeSqrt() const;
        float   Max() const;
        float   Min() const;
        float   Avg() const;
        void    Normalize();

        Vector2& operator*=(Vector2 const& v)
        {
            x *= v.x;
            y *= v.y;
            return *this;
        }
        Vector2& operator/=(Vector2 const& v)
        {
            x /= v.x;
            y /= v.y;
            return *this;
        }
        Vector2& operator+=(Vector2 const& v)
        {
            x += v.x;
            y += v.y;
            return *this;
        }
        Vector2& operator-=(Vector2 const& v)
        {
            x -= v.x;
            y -= v.y;
            return *this;
        }
        Vector2& operator*=(float f)
        {
            x *= f;
            y *= f;
            return *this;
        }
        Vector2& operator/=(float f)
        {
            x /= f;
            y /= f;
            return *this;
        }
        Vector2& operator+=(float f)
        {
            x += f;
            y += f;
            return *this;
        }
        Vector2& operator-=(float f)
        {
            x -= f;
            y -= f;
            return *this;
        }
        bool operator==(const Vector2& rhs) const
        {
            return x == rhs.x && y == rhs.y;
        }
        bool operator!=(const Vector2& rhs) const
        {
            return !(x == rhs.x && y == rhs.y);
        }
        bool operator>(const Vector2& rhs) const
        {
            return length() > rhs.length();
        }
        bool operator<(const Vector2& rhs) const
        {
            return length() < rhs.length();
        }
        float& operator[](unsigned int i)
        {
            return (&x)[i];
        }
        Vector2 operator-() const
        {
            return Vector2(-*this);
        }
        float* Get()
        {
            return &x;
        }

        std::ostream& operator<<(std::ostream& os)
        {
            return os << "X:" << x << "   Y:" << y;
        }

        std::string ToString() const;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(x, y);
        }
    };
}