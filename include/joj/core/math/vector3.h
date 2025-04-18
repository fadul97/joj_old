#ifndef _JOJ_VECTOR3_H
#define _JOJ_VECTOR3_H

#include "joj/core/defines.h"

#include <cmath>
#include "joj/core/string_utils.h"

#if JOJ_PLATFORM_WINDOWS
#include "DirectXMath.h"
#endif

namespace joj
{
    class JOJ_API Vector3
    {
    public:
        // Constructors
        Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
        Vector3(f32 value) : x(value), y(value), z(value) {}
        Vector3(f32 x, f32 y, f32 z) : x(x), y(y), z(z) {}

#if JOJ_PLATFORM_WINDOWS
        Vector3(const DirectX::XMFLOAT3& v) : x(v.x), y(v.y), z(v.z) {}
#endif

        // Binary operators
        Vector3 operator+(const Vector3& other) const { return Vector3(x + other.x, y + other.y, z + other.z); }
        Vector3 operator-(const Vector3& other) const { return Vector3(x - other.x, y - other.y, z - other.z); }
        Vector3 operator*(const Vector3& other) const { return Vector3(x * other.x, y * other.y, z * other.z); }
        Vector3 operator/(const Vector3& other) const { return Vector3(x / other.x, y / other.y, z / other.z); }

        // Unary operators
        Vector3 operator+(f32 value) const { return Vector3(x + value, y + value, z + value); }
        Vector3 operator-(f32 value) const { return Vector3(x - value, y - value, z - value); }
        Vector3 operator*(f32 value) const { return Vector3(x * value, y * value, z * value); }
        Vector3 operator/(f32 value) const { return Vector3(x / value, y / value, z / value); }

        // Attribution operators
        Vector3& operator+=(const Vector3& other) { x += other.x; y += other.y; z += other.z; return *this; }
        Vector3& operator-=(const Vector3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
        Vector3& operator*=(const Vector3& other) { x *= other.x; y *= other.y; z *= other.z; return *this; }
        Vector3& operator/=(const Vector3& other) { x /= other.x; y /= other.y; z /= other.z; return *this; }

        Vector3& operator+=(f32 value) { x += value; y += value; z += value; return *this; }
        Vector3& operator-=(f32 value) { x -= value; y -= value; z -= value; return *this; }
        Vector3& operator*=(f32 value) { x *= value; y *= value; z *= value; return *this; }
        Vector3& operator/=(f32 value) { x /= value; y /= value; z /= value; return *this; }

        // Comparing operators
        bool operator==(const Vector3& other) const { return x == other.x && y == other.y && z == other.z; }
        bool operator!=(const Vector3& other) const { return !(*this == other); }

        // Vector methods
        f32 length() const { return std::sqrt(x * x + y * y + z * z); }
        f32 length_squared() const { return x * x + y * y + z * z; }
        f32 dot(const Vector3& other) const { return x * other.x + y * other.y + z * other.z; }
        Vector3 cross(const Vector3& other) const
        {
            return Vector3(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
            );
        }

        // Normalization
        Vector3 normalized() const
        {
            f32 len = length();
            return (len > 0) ? *this / len : Vector3();
        }

        void normalize()
        {
            f32 len = length();
            if (len > 0)
            {
                *this /= len;
            }
        }

        const char* to_string() const
        {
            constexpr i32 buffer_size = 64;
            static char buffer[buffer_size];
            StringUtils::format(buffer, buffer_size, "(%.3f, %.3f, %.3f)", x, y, z);
            return buffer;
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
        void from_XMVECTOR(const DirectX::FXMVECTOR& v)
        {
            x = DirectX::XMVectorGetX(v);
            y = DirectX::XMVectorGetY(v);
            z = DirectX::XMVectorGetZ(v);
        }
#endif
    };

#if JOJ_PLATFORM_WINDOWS
    inline DirectX::XMFLOAT3 Vector3_to_XMFLOAT3(const Vector3& v) { return DirectX::XMFLOAT3(v.x, v.y, v.z); }
    inline Vector3 XMFLOAT3_to_Vector3(const DirectX::XMFLOAT3& v) { return Vector3(v.x, v.y, v.z); }
    inline b8 is_Vector3_equal_to_XMFLOAT3(const Vector3& v1, const DirectX::XMFLOAT3& v2, f32 epsilon = 0.0001f)
    {
        return std::abs(v1.x - v2.x) < epsilon &&
               std::abs(v1.y - v2.y) < epsilon &&
               std::abs(v1.z - v2.z) < epsilon;
    }
    inline Vector3 XMVECTOR_to_Vector3(const DirectX::FXMVECTOR& v)
    {
        return Vector3(
            DirectX::XMVectorGetX(v),
            DirectX::XMVectorGetY(v),
            DirectX::XMVectorGetZ(v)
        );
    }
#endif
}

#endif // _JOJ_VECTOR3_H
