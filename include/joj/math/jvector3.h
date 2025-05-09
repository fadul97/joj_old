#ifndef _JOJ_VECTOR3_H
#define _JOJ_VECTOR3_H

#include "joj/core/defines.h"

#include <cmath>

#if JOJ_PLATFORM_WINDOWS
#include "DirectXMath.h"
#endif

namespace joj
{
    class JOJ_API JVector3
    {
    public:
        // Constructors
        JVector3() : x(0.0f), y(0.0f), z(0.0f) {}
        JVector3(f32 value) : x(value), y(value), z(value) {}
        JVector3(f32 x, f32 y, f32 z) : x(x), y(y), z(z) {}

#if JOJ_PLATFORM_WINDOWS
        JVector3(const DirectX::XMFLOAT3& v) : x(v.x), y(v.y), z(v.z) {}
#endif

        // Binary operators
        JVector3 operator+(const JVector3& other) const { return JVector3(x + other.x, y + other.y, z + other.z); }
        JVector3 operator-(const JVector3& other) const { return JVector3(x - other.x, y - other.y, z - other.z); }
        JVector3 operator*(const JVector3& other) const { return JVector3(x * other.x, y * other.y, z * other.z); }
        JVector3 operator/(const JVector3& other) const { return JVector3(x / other.x, y / other.y, z / other.z); }

        // Unary operators
        JVector3 operator+(f32 value) const { return JVector3(x + value, y + value, z + value); }
        JVector3 operator-(f32 value) const { return JVector3(x - value, y - value, z - value); }
        JVector3 operator*(f32 value) const { return JVector3(x * value, y * value, z * value); }
        JVector3 operator/(f32 value) const { return JVector3(x / value, y / value, z / value); }

        // Attribution operators
        JVector3& operator+=(const JVector3& other) { x += other.x; y += other.y; z += other.z; return *this; }
        JVector3& operator-=(const JVector3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
        JVector3& operator*=(const JVector3& other) { x *= other.x; y *= other.y; z *= other.z; return *this; }
        JVector3& operator/=(const JVector3& other) { x /= other.x; y /= other.y; z /= other.z; return *this; }

        JVector3& operator+=(f32 value) { x += value; y += value; z += value; return *this; }
        JVector3& operator-=(f32 value) { x -= value; y -= value; z -= value; return *this; }
        JVector3& operator*=(f32 value) { x *= value; y *= value; z *= value; return *this; }
        JVector3& operator/=(f32 value) { x /= value; y /= value; z /= value; return *this; }

        // Comparing operators
        bool operator==(const JVector3& other) const { return x == other.x && y == other.y && z == other.z; }
        bool operator!=(const JVector3& other) const { return !(*this == other); }

        // Vector methods
        f32 length() const { return std::sqrt(x * x + y * y + z * z); }
        f32 length_squared() const { return x * x + y * y + z * z; }
        f32 dot(const JVector3& other) const { return x * other.x + y * other.y + z * other.z; }
        JVector3 cross(const JVector3& other) const
        {
            return JVector3(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
            );
        }

        // Normalization
        JVector3 normalized() const
        {
            f32 len = length();
            return (len > 0) ? *this / len : JVector3();
        }

        void normalize()
        {
            f32 len = length();
            if (len > 0)
            {
                *this /= len;
            }
        }

        // TODO: Union to access to components as RGB or UVW
        f32 x;
        f32 y;
        f32 z;


#if JOJ_PLATFORM_WINDOWS
        DirectX::XMFLOAT3 to_XMFLOAT3() const { return DirectX::XMFLOAT3(x, y, z); }
        void from_XMFLOAT3(const DirectX::XMFLOAT3& other) { x = other.x; y = other.y; z = other.z; }
        b8 operator==(const DirectX::XMFLOAT3& other) const { return x == other.x && y == other.y && z == other.z; }
        b8 is_equal_to_XMFLOAT3(const DirectX::XMFLOAT3& other, f32 epsilon = 0.0001f) const
        {
            return std::abs(x - other.x) < epsilon &&
                   std::abs(y - other.y) < epsilon &&
                   std::abs(z - other.z) < epsilon;
        }
#endif
    };

#if JOJ_PLATFORM_WINDOWS
    inline DirectX::XMFLOAT3 JVector3_to_XMFLOAT3(const JVector3& v) { return DirectX::XMFLOAT3(v.x, v.y, v.z); }
    inline JVector3 XMFLOAT3_to_JVector3(const DirectX::XMFLOAT3& v) { return JVector3(v.x, v.y, v.z); }
    inline b8 is_JVector3_equal_to_XMFLOAT3(const JVector3& v1, const DirectX::XMFLOAT3& v2, f32 epsilon = 0.0001f)
    {
        return std::abs(v1.x - v2.x) < epsilon &&
               std::abs(v1.y - v2.y) < epsilon &&
               std::abs(v1.z - v2.z) < epsilon;
    }
#endif
}

#endif // _JOJ_VECTOR3_H