/*!
@Affine.cpp
@author JiMin Lim
@course CS200
@Affine and Mesh Programming Assignment
@date 26/10/2023
@brief Provides 3D coordinate and matrix operations and includes a variety of transformations, including rotation, movement, and scaling.
*/

#include "Affine.h"

// Hcoord constructors
Hcoord::Hcoord(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {}

// Point constructors
Point::Point(float X, float Y, float Z) : Hcoord(X, Y, Z, 1) {}

// Vector constructors
Vector::Vector(float X, float Y, float Z) : Hcoord(X, Y, Z, 0) {}

// Affine constructors
Affine::Affine(void)
{
    row[0] = Vector(1, 0, 0);
    row[1] = Vector(0, 1, 0);
    row[2] = Vector(0, 0, 1);
    row[3] = Point(0, 0, 0);
}

Affine::Affine(const Vector& Lx, const Vector& Ly, const Vector& Lz, const Point& D)
{
    row[0] = Hcoord(Lx.x, Ly.x, Lz.x, D.x);
    row[1] = Hcoord(Lx.y, Ly.y, Lz.y, D.y);
    row[2] = Hcoord(Lx.z, Ly.z, Lz.z, D.z);
    row[3] = Hcoord(Lx.w, Ly.w, Lz.w, D.w);
}

// Operations on Hcoords
Hcoord operator+(const Hcoord& u, const Hcoord& v)
{
    return Hcoord(u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w);
}

Hcoord operator-(const Hcoord& u, const Hcoord& v)
{
    return Hcoord(u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w);
}

Hcoord operator-(const Hcoord& v)
{
    return Hcoord(-v.x, -v.y, -v.z, -v.w);
}

Hcoord operator*(float r, const Hcoord& v)
{
    return Hcoord(r * v.x, r * v.y, r * v.z, r * v.w);
}

Hcoord operator*(const Matrix& A, const Hcoord& v)
{
    Hcoord result;
    for (int i = 0; i < 4; i++)
    {
        result[i] = A[i][0] * v[0] + A[i][1] * v[1] + A[i][2] * v[2] + A[i][3] * v[3];
    }

    return result;
}

Matrix operator*(const Matrix& A, const Matrix& B)
{
    Matrix result;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            result[i][j] = A[i][0] * B[0][j] + A[i][1] * B[1][j] + A[i][2] * B[2][j] + A[i][3] * B[3][j];
        }
    }
    return result;
}

float dot(const Vector& u, const Vector& v)
{
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

float abs(const Vector& v)
{
    return std::sqrt(dot(v, v));
}

Vector normalize(const Vector& v)
{
    float len = abs(v);
    return Vector(v.x / len, v.y / len, v.z / len);
}

Vector cross(const Vector& u, const Vector& v)
{
    return Vector(
        u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x);
}

Affine rotate(float t, const Vector& v)
{
    Vector axis = normalize(v);

    float cos_t = cos(t);
    float sin_t = sin(t);

    float one_minus_cos_t = 1.0f - cos_t;

    float rotationMatrix[4][4] = {
        {cos_t + axis.x * axis.x * one_minus_cos_t, axis.x * axis.y * one_minus_cos_t - axis.z * sin_t, axis.x * axis.z * one_minus_cos_t + axis.y * sin_t, 0.0f},
        {axis.y * axis.x * one_minus_cos_t + axis.z * sin_t, cos_t + axis.y * axis.y * one_minus_cos_t, axis.y * axis.z * one_minus_cos_t - axis.x * sin_t, 0.0f},
        {axis.z * axis.x * one_minus_cos_t - axis.y * sin_t, axis.z * axis.y * one_minus_cos_t + axis.x * sin_t, cos_t + axis.z * axis.z * one_minus_cos_t, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f} };

    Affine rotationAffine(
        Vector(rotationMatrix[0][0], rotationMatrix[1][0], rotationMatrix[2][0]),
        Vector(rotationMatrix[0][1], rotationMatrix[1][1], rotationMatrix[2][1]),
        Vector(rotationMatrix[0][2], rotationMatrix[1][2], rotationMatrix[2][2]),
        Point(0, 0, 0));

    return rotationAffine;

}

Affine translate(const Vector& v)
{
    Affine trans;
    trans[0][3] = v.x;
    trans[1][3] = v.y;
    trans[2][3] = v.z;
    return trans;
}

Affine scale(float r)
{
    return Affine(Vector(r, 0, 0),
        Vector(0, r, 0),
        Vector(0, 0, r),
        Point(0, 0, 0));
}

Affine scale(float rx, float ry, float rz)
{
    return Affine(Vector(rx, 0, 0), Vector(0, ry, 0), Vector(0, 0, rz), Point(0, 0, 0));
}

Affine inverse(const Affine& A)
{
    Vector Lx = Vector(A[0][0], A[1][0], A[2][0]);
    Vector Ly = Vector(A[0][1], A[1][1], A[2][1]);
    Vector Lz = Vector(A[0][2], A[1][2], A[2][2]);
    Point D = Point(A[0][3], A[1][3], A[2][3]);

    float det = Lx.x * (Ly.y * Lz.z - Ly.z * Lz.y) - Lx.y * (Ly.x * Lz.z - Ly.z * Lz.x) + Lx.z * (Ly.x * Lz.y - Ly.y * Lz.x);

    if (std::fabs(det) < 1e-6)
    {
        return Affine(Vector(1, 0, 0), Vector(0, 1, 0), Vector(0, 0, 1), Point(0, 0, 0));
    }

    float invDet = 1.0f / det;

    Vector invLx = Vector(
        (Ly.y * Lz.z - Ly.z * Lz.y) * invDet,
        (Lx.z * Lz.y - Lx.y * Lz.z) * invDet,
        (Lx.y * Ly.z - Lx.z * Ly.y) * invDet);

    Vector invLy = Vector(
        (Ly.z * Lz.x - Ly.x * Lz.z) * invDet,
        (Lx.x * Lz.z - Lx.z * Lz.x) * invDet,
        (Ly.x * Lx.z - Lx.x * Ly.z) * invDet);

    Vector invLz = Vector(
        (Ly.x * Lz.y - Ly.y * Lz.x) * invDet,
        (Lx.y * Lz.x - Lx.x * Lz.y) * invDet,
        (Lx.x * Ly.y - Lx.y * Ly.x) * invDet);

    Point invD = Point(
        -D.x * invLx.x - D.y * invLy.x - D.z * invLz.x,
        -D.x * invLx.y - D.y * invLy.y - D.z * invLz.y,
        -D.x * invLx.z - D.y * invLy.z - D.z * invLz.z);

    Affine inverseMatrix(invLx, invLy, invLz, invD);

    return inverseMatrix;
}




