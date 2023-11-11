#pragma once
#include "glm.h"
#include <sstream>
#include "Serialization.h"

namespace Odyssey
{
    using namespace glm;
    class Vector2 : public glm::vec2
    {
    public:
        Vector2();
        Vector2(float x, float y);
        Vector2(const Vector2& rhs);
        Vector2(float val);
        Vector2(const glm::vec2& rhs);

    public:
        Vector2& operator*=(Vector2 const& v);
        Vector2& operator/=(Vector2 const& v);
        Vector2& operator+=(Vector2 const& v);
        Vector2& operator-=(Vector2 const& v);
        Vector2& operator*=(float f);
        Vector2& operator/=(float f);
        Vector2& operator+=(float f);
        Vector2& operator-=(float f);
        bool operator==(const Vector2& rhs) const;
        bool operator!=(const Vector2& rhs) const;
        bool operator>(const Vector2& rhs) const;
        bool operator<(const Vector2& rhs) const;
        float& operator[](unsigned int i);
        Vector2 operator-() const;
        std::ostream& operator<<(std::ostream& os);

    public:
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
        float* Get();
        std::string ToString() const;

    public:
        static Vector2 Zero;
        static Vector2 One;
        ODYSSEY_SERIALIZE(Vector2, x, y);
    };
}