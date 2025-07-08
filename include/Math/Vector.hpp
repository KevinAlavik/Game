#pragma once

namespace Math {
    class Vector2f {
    public:
        float x;
        float y;

        Vector2f();
        Vector2f(float x, float y);

        Vector2f operator+(const Vector2f& rhs) const;
        Vector2f operator-(const Vector2f& rhs) const;
        Vector2f& operator+=(const Vector2f& rhs);
        Vector2f& operator-=(const Vector2f& rhs);
        Vector2f operator*(float scalar) const;
        Vector2f& operator*=(float scalar);

        bool operator==(const Vector2f& rhs) const;
        bool operator!=(const Vector2f& rhs) const;
    };

    class Vector3f {
    public:
        float x;
        float y;
        float z;

        Vector3f();
        Vector3f(float x, float y, float z);

        Vector3f operator+(const Vector3f& rhs) const;
        Vector3f operator-(const Vector3f& rhs) const;
        Vector3f& operator+=(const Vector3f& rhs);
        Vector3f& operator-=(const Vector3f& rhs);
        Vector3f operator*(float scalar) const;
        Vector3f& operator*=(float scalar);

        bool operator==(const Vector3f& rhs) const;
        bool operator!=(const Vector3f& rhs) const;
    };

    class Vector4f {
    public:
        float x;
        float y;
        float z;
        float w;

        Vector4f();
        Vector4f(float x, float y, float z, float w);

        Vector4f operator+(const Vector4f& rhs) const;
        Vector4f operator-(const Vector4f& rhs) const;
        Vector4f& operator+=(const Vector4f& rhs);
        Vector4f& operator-=(const Vector4f& rhs);
        Vector4f operator*(float scalar) const;
        Vector4f& operator*=(float scalar);

        bool operator==(const Vector4f& rhs) const;
        bool operator!=(const Vector4f& rhs) const;
    };
}
