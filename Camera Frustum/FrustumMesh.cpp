/*!
@FrustumMesh.cpp
@author JiMin Lim
@course CS200
@Camera Frustum Programming Assignment
@date 14/11/2023
@brief This program defines a class that calculates the shape of a proustum with specific parameters and provides information about it.
*/


#include "FrustumMesh.h"
#include <cmath>

const Mesh::Edge FrustumMesh::edges[16] =
{

    {0, 1},
    {1, 2},
    {2, 3},
    {3, 0},

    {4, 5},
    {5, 6},
    {6, 7},
    {7, 4},

    {0, 4},
    {1, 5},
    {2, 6},
    {3, 7},

    {8, 0},
    {8, 1},
    {8, 2},
    {8, 3}
};

const Mesh::Face FrustumMesh::faces[12] =
{

    {0, 1, 2 },
    { 2, 3, 0},

    { 4, 7, 6},
    { 6, 5, 4},

    { 0, 4, 5},
    { 5, 1, 0},
    { 1, 5, 6},
    { 6, 2, 1},
    { 2, 6, 7},
    { 7, 3, 2},
    { 3, 7, 4},
    {4, 0, 3}
};

FrustumMesh::FrustumMesh(float fov, float a, float n, float f)
{
    float tanFov = std::tan(fov / 2.0f);
    float nh = n * tanFov;
    float nw = nh * a;
    float fh = f * tanFov;
    float fw = fh * a;

    // Near and far planes
    vertices[0] = Point(-nw, -nh, -n);
    vertices[1] = Point(nw, -nh, -n);
    vertices[2] = Point(nw, nh, -n);
    vertices[3] = Point(-nw, nh, -n);
    vertices[4] = Point(-fw, -fh, -f);
    vertices[5] = Point(fw, -fh, -f);
    vertices[6] = Point(fw, fh, -f);
    vertices[7] = Point(-fw, fh, -f);

    vertices[8] = Point(0, 0, 0);

    center = Point(0, 0, -(n + f) / 2);

    dimensions = Vector(fw * 2, fh * 2, f - n);
}

/*!
@brief eturns the number of vertex in the pristine mesh (fixed value 9).
@param void
@return int
*/
/*______________________________________________________________*/
int FrustumMesh::VertexCount(void)
{
    return 9;
}

/*!
@brief Returns the vertex of the pristine mesh corresponding to index i.
@param int i
@return Point
*/
/*______________________________________________________________*/
Point FrustumMesh::GetVertex(int i)
{
    return vertices[i];
}

/*!
@brief Returns a vector representing the dimension (size) of the Prost mesh.
@param void
@return Vector
*/
/*______________________________________________________________*/
Vector FrustumMesh::Dimensions(void)
{
    return dimensions;
}

/*!
@brief Returns the center coordinate of the pristine mesh.
@param void
@return Point
*/
/*______________________________________________________________*/
Point FrustumMesh::Center(void)
{
    return center;
}

/*!
@brief Returns the number of faces in the pristine mesh (fixed value 12).
@param void
@return int
*/
/*______________________________________________________________*/
int FrustumMesh::FaceCount(void)
{
    return 12;
}

/*!
@brief Returns the plane of the pristine mesh corresponding to index i.
@param int i
@return Face
*/
/*______________________________________________________________*/
Mesh::Face FrustumMesh::GetFace(int i)
{
    return faces[i];
}

/*!
@brief Returns the number of edges in the pristine mesh (fixed value 16).
@param void
@return int
*/
/*______________________________________________________________*/
int FrustumMesh::EdgeCount(void)
{
    return 16;
}

/*!
@brief Returns the edge of the pristine mesh corresponding to index i.
@param int i
@return Edge
*/
/*______________________________________________________________*/
Mesh::Edge FrustumMesh::GetEdge(int i)
{
    return edges[i];
}