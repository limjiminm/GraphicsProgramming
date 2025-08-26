#include "Affine.h"
#include <limits>
#include <stdexcept>

Hcoord::Hcoord(float X, float Y, float Z, float W)
{
    x = X;
    y = Y;
    z = Z;
    w = W;
}

Point::Point(float X, float Y, float Z) : Hcoord(X, Y, Z, 1.0)
{}

Vector::Vector(float X, float Y, float Z) : Hcoord(X, Y, Z, 0.0)
{}

Affine::Affine(void)
{
    row[0] = Hcoord(1, 0, 0, 0);
    row[1] = Hcoord(0, 1, 0, 0);
    row[2] = Hcoord(0, 0, 1, 0);
    row[3] = Hcoord(0, 0, 0, 1);
}

Affine::Affine(const Vector& Lx, const Vector& Ly, const Vector& Lz, const Point& D) //벡터 하나당 포인트를 3개
{
    row[0] = Hcoord(Lx.x, Ly.x, Lz.x, D.x);
    row[1] = Hcoord(Lx.y, Ly.y, Lz.y, D.y);
    row[2] = Hcoord(Lx.z, Ly.z, Lz.z, D.z);
    row[3] = Hcoord(0, 0, 0, 1);
}

Hcoord operator+(const Hcoord& u, const Hcoord& v) //매개변수는 각각 4차원 좌표계를 의미
{
    Hcoord new_coord;
    new_coord.x = u.x + v.x;
    new_coord.y = u.y + v.y;
    new_coord.z = u.z + v.z;
    new_coord.w = u.w + v.w;
    return new_coord;
}

Hcoord operator-(const Hcoord& u, const Hcoord& v)
{
    Hcoord new_coord;
    new_coord.x = u.x - v.x;
    new_coord.y = u.y - v.y;
    new_coord.z = u.z - v.z;
    new_coord.w = u.w - v.w;
    return new_coord;
}

Hcoord operator-(const Hcoord& v)
{
    Hcoord new_coord;
    new_coord.x = -v.x;
    new_coord.y = -v.y;
    new_coord.z = -v.z;
    new_coord.w = -v.w;
    return new_coord;
}

Hcoord operator*(float r, const Hcoord& v)
{
    Hcoord new_coord;
    new_coord.x = r * v.x;
    new_coord.y = r * v.y;
    new_coord.z = r * v.z;
    new_coord.w = r * v.w;
    return new_coord;
}

Hcoord operator*(const Matrix& A, const Hcoord& v)
{
    Hcoord new_coord;
    new_coord.x = A[0][0] * v.x + A[0][1] * v.y + A[0][2] * v.z + A[0][3] * v.w;
    new_coord.y = A[1][0] * v.x + A[1][1] * v.y + A[1][2] * v.z + A[1][3] * v.w;
    new_coord.z = A[2][0] * v.x + A[2][1] * v.y + A[2][2] * v.z + A[2][3] * v.w;
    new_coord.w = A[3][0] * v.x + A[3][1] * v.y + A[3][2] * v.z + A[3][3] * v.w;
    return new_coord;
}

Matrix operator*(const Matrix& A, const Matrix& B)
{
    Affine C;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            C[i][j] = 0;
            for (int k = 0; k < 4; k++)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return C;
}

Matrix operator*(const Matrix& A, float r)
{
    Affine C;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            C[i][j] = A[i][j] * r;
        }
    }
    return C;
}

Matrix operator+(const Matrix& A, const Matrix& B)
{
    Affine C;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            C[i][j] = A[i][j] + B[i][j];
        }
    }
    return C;
}

float dot(const Vector& u, const Vector& v)
{
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

float abs(const Vector& v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vector normalize(const Vector& v)
{
    Vector new_vec;
    new_vec = (1 / abs(v)) * v;
    return new_vec;
}

Vector cross(const Vector& u, const Vector& v)
{
    Vector new_vec;
    new_vec.x = u.y * v.z - u.z * v.y;
    new_vec.y = u.z * v.x - u.x * v.z;
    new_vec.z = u.x * v.y - u.y * v.x;
    return new_vec;
}

Affine rotate(float t, const Vector& v)
{
    Vector n = normalize(v);

    float cos_theta = std::cos(t);
    float sin_theta = std::sin(t);

    Affine result;
    Affine uuT;

    uuT[0] = Hcoord(n.x * n.x, n.x * n.y, n.x * n.z, 0);
    uuT[1] = Hcoord(n.y * n.x, n.y * n.y, n.y * n.z, 0);
    uuT[2] = Hcoord(n.z * n.x, n.z * n.y, n.z * n.z, 0);
    uuT[3] = Hcoord(0, 0, 0, 1);


    Vector Lx(0, n.z, -n.y);
    Vector Ly(-n.z, 0, n.x);
    Vector Lz(n.y, -n.x, 0);
    Point D(0, 0, 0);

    Affine N(Lx, Ly, Lz, D);
    result = result * cos_theta + uuT * (1.0f - cos_theta) + N * sin_theta;

    return result;
}

Affine scale(float r)
{
    Affine result;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (i == j)
            {
                result[i][j] = r;
            }
        }
    }
    return result;
}

Affine scale(float rx, float ry, float rz)
{
    Affine result;
    result[0][0] = rx;
    result[1][1] = ry;
    result[2][2] = rz;
    return result;
}

Affine translate(const Vector& v)
{
    Affine result;
    result[0][3] = v.x;
    result[1][3] = v.y;
    result[2][3] = v.z;
    return result;
}

Point Center(const Affine& affine) {
    Vector translation(affine[0][3], affine[1][3], affine[2][3]);
    return Point(translation);
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