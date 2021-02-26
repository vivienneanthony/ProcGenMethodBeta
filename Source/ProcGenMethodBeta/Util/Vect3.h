#pragma once

// Vivienne Anthony
// 2020

class Vect3
{
public:
    // variables
    float x;
    float y;
    float z;

    // constructor and deconstructor
    Vect3()
    {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    };

    Vect3(float inX, float inY, float inZ)
    {
        x = inX;
        y = inY;
        z = inZ;
    };

    ~Vect3(){};

    /// Add-assign a vector.
    Vect3 &operator+=(const Vect3 &rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    };

    /// Subtract-assign a vector.
    Vect3 &operator-=(const Vect3 &rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    };

    /// Multiply-assign a scalar.
    Vect3 &operator*=(int rhs)
    {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        return *this;
    };

    /// Multiply-assign a vector.
    Vect3 &operator*=(const Vect3 &rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return *this;
    };

    /// Divide-assign a vector.
    Vect3 &operator/=(const Vect3 &rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        return *this;
    };

    /// Divide-assign a scalar.
    Vect3 &operator/=(const int &rhs)
    {
        x /= (float)rhs;
        y /= (float)rhs;
        z /= (float)rhs;
        return *this;
    };

    /// Divide-assign a scalar.
    Vect3 &operator/=(const float &rhs)
    {
        x /= rhs;
        y /= rhs;
        z /= rhs;
        return *this;
    };

    /// Subtract operator / Vector
    Vect3 operator-(const Vect3 &rhs)
    {
        return Vect3(x - rhs.x, y - rhs.y, z - rhs.z);
    };

    /// Additonial operator / Vector
    Vect3 operator+(const Vect3 &rhs)
    {
        return Vect3(x + rhs.x, y + rhs.y, z + rhs.z);
    };

    /// Multiplication operator / Vector
    Vect3 operator*(const Vect3 &rhs)
    {
        return Vect3(x * rhs.x, y * rhs.y, z * rhs.z);
    };

    /// Divide operator / Vector
    Vect3 operator/(const Vect3 &rhs)
    {
        return Vect3(x / rhs.x, y / rhs.y, z / rhs.z);
    };

    // Divide Operator
    Vect3 operator/(const float &rhs)
    {
        return Vect3(x / rhs, y / rhs, z / rhs);
    };

    // Multiplication Operator
    Vect3 operator*(const float &rhs)
    {
        return Vect3(x * rhs, y * rhs, z * rhs);
    };

    // Multiplication Operator
    Vect3 operator-(const float &rhs)
    {
        return Vect3(x * rhs, y * rhs, z * rhs);
    };

    // Addition Operator
    Vect3 operator+(const float &rhs)
    {
        return Vect3(x + rhs, y + rhs, z + rhs);
    };

    float CalculateDistance(const Vect3 &rhs)
    {
        return std::sqrt(((x - rhs.x) * (x - rhs.x)) + ((y - rhs.y) * (y - rhs.y)) + ((z - rhs.z) * (z - rhs.z)));
    };

    // Cubize Point Algorithm
    Vect3 cubizePoint();

    // ProjectCubeToSphere
    void ProjectCubizeToCubeXYZ();
};

class PointV3
{
public:
    // constructor and deconstructor
    PointV3()
    {
        v = Vect3(0.0f, 0.0f, 0.0f);
        n = Vect3(0.0f, 0.0f, 0.0f);
    };

    ~PointV3(){

    };

    Vect3 v;
    Vect3 n;
};
