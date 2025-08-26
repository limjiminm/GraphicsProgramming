// HalfSpace.cpp
// -- implementation of half-space package
// cs250 3/17

/*!
@HalfSpace.cpp
@author JiMin Lim
@course CS200
@Clipping Programming Assignment
@date 1/12/2023
@brief effectively represents planes and different polyhedrons in space and verifies that the given point is located inside the polyhedron
*/


#include "HalfSpace.h"

/*!
@brief A generator that defines a plane using a given normal vector and points.
       Set the components of the normal vector, and calculate the value of w representing the equation of the plane.
@param const Vector& m, const Point& C
@return
*/
/*______________________________________________________________*/
HalfSpace::HalfSpace(const Vector& m, const Point& C)
{
    x = m.x;
    y = m.y;
    z = m.z;
    w = -(m.x * C.x + m.y * C.y + m.z * C.z + m.w * C.w);
}

/*!
@brief A generator that defines a plane by using three points and one point.
       Calculate the normal vector of the plane with the three points given, and calculate the value of w with the point where the plane passes.
@param const Point& A, const Point& B, const Point& C, const Point& P
@return
*/
/*______________________________________________________________*/
HalfSpace::HalfSpace(const Point& A, const Point& B, const Point& C, const Point& P) {
    Vector normal = cross(B - A, C - A);
    x = normal.x;
    y = normal.y;
    z = normal.z;
    w = -dot(*this, Hcoord(A.x, A.y, A.z, 1.0));

    if (dot(*this, Hcoord(P.x, P.y, P.z, 1.0)) >= 0)
    {
        x = -x;
        y = -y;
        z = -z;
        w = -w;
    }
}

/*!
@brief The creator who creates the cube. Create six half-planes in the +x, -x, +y, -y, +z, and -z directions and add them to the Polyhedron object.
@param bool cube
@return
*/
/*______________________________________________________________*/
Polyhedron::Polyhedron(bool cube)
{
    if (cube)
    {

        half_spaces.push_back({ -1, 0, 0, -1 });  // +x
        half_spaces.push_back({ 1, 0, 0, -1 }); // -x

        half_spaces.push_back({ 0, -1, 0, -1 });  // +y
        half_spaces.push_back({ 0, 1, 0, -1 }); // -y

        half_spaces.push_back({ 0, 0, -1, -1 });  // +z
        half_spaces.push_back({ 0, 0, 1, -1 }); // -z 
    }
}

/*!
@brief Generators that use perspective projections to create perspective projections
@param float fov, float a, float N, float F
@return
*/
/*______________________________________________________________*/
Polyhedron::Polyhedron(float fov, float a, float N, float F)
{
    float D = F - N;
    float W = 2 * D * tan(fov / 2);
    float H = W / a;

    float WN = (N / D) * W;
    float HN = H / W * WN;

    float WF = (F / D) * W;
    float HF = H / W * WF;

    float halfWN = WN / 2;
    float halfHN = HN / 2;

    Hcoord nearVertices[4];
    nearVertices[0] = Hcoord(-halfWN, halfHN, -N, 0);
    nearVertices[1] = Hcoord(halfWN, halfHN, -N, 0);
    nearVertices[2] = Hcoord(-halfWN, -halfHN, -N, 0);
    nearVertices[3] = Hcoord(halfWN, -halfHN, -N, 0);

    float halfWF = WF / 2;
    float halfHF = HF / 2;

    Hcoord farVertices[4];
    farVertices[0] = Hcoord(-halfWF, halfHF, -F, 0);
    farVertices[1] = Hcoord(halfWF, halfHF, -F, 0);
    farVertices[2] = Hcoord(-halfWF, -halfHF, -F, 0);
    farVertices[3] = Hcoord(halfWF, -halfHF, -F, 0);

    Vector direction1(
        farVertices[1].x - nearVertices[1].x,
        farVertices[1].y - nearVertices[1].y,
        farVertices[1].z - nearVertices[1].z
    );

    Vector direction2(
        nearVertices[2].x - nearVertices[1].x,
        nearVertices[2].y - nearVertices[1].y,
        nearVertices[2].z - nearVertices[1].z
    );

    Vector normalNear = cross(direction1, direction2);

    HalfSpace nearPlane(
        normalNear.x,
        normalNear.y,
        normalNear.z,
        -dot(normalNear, Vector(nearVertices[1].x, nearVertices[1].y, nearVertices[1].z))
    );
    half_spaces.push_back(nearPlane);

}

/*!
@brief A function that determines whether a given Polyhedron contains a given point P.
       Check all planes belonging to the Polyhedron object, and return whether the point is inside for all planes.
@param const Polyhedron& polyhedron, const Hcoord& P
@return bool
*/
/*______________________________________________________________*/
bool contains(const Polyhedron& polyhedron, const Hcoord& P)
{
    if (P.w > 0)
    {
        for (const HalfSpace& h : polyhedron.half_spaces)
        {
            if (dot(h, P) >= 0)
            {
                return false;
            }
        }
        return true;
    }
    return false;
}