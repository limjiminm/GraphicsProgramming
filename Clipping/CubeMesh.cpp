/*!
@CubeMesh.cpp
@author JiMin Lim
@course CS200
@Affine and Mesh Programming Assignment
@date 26/10/2023
@brief It covers vertex, face, and edge information in the cube mesh, returns the number of vertices, faces, and edges,
and gives the dimension and center of the cube as fixed values.
*/


#include "CubeMesh.h"

const Point CubeMesh::vertices[8] = {
    Point(-1, -1, -1),
    Point(-1, -1, 1),
    Point(-1, 1, -1),
    Point(-1, 1, 1),
    Point(1, -1, -1),
    Point(1, -1, 1),
    Point(1, 1, -1),
    Point(1, 1, 1)
};

const CubeMesh::Face CubeMesh::faces[12] = {
    // Front
    Face(0, 1, 3),
    Face(0, 3, 2),

    // Right
    Face(1, 5, 7),
    Face(1, 7, 3),

    // Back
    Face(5, 4, 6),
    Face(5, 6, 7),

    // Left
    Face(4, 0, 2),
    Face(4, 2, 6),

    // Top
    Face(2, 3, 7),
    Face(2, 7, 6),

    // Bottom
    Face(4, 5, 1),
    Face(4, 1, 0)
};


const CubeMesh::Edge CubeMesh::edges[12] = {
    Edge(0, 2),
    Edge(3, 2),
    Edge(1, 3),
    Edge(0, 1),
    Edge(5, 1),
    Edge(4, 5),
    Edge(0, 4),
    Edge(6, 4),
    Edge(2, 6),
    Edge(5, 7),
    Edge(6, 7),
    Edge(3, 7)
};

int CubeMesh::VertexCount(void) {
    return sizeof(vertices) / sizeof(vertices[0]);
}
Point CubeMesh::GetVertex(int i) {
    return vertices[i];
}
Vector CubeMesh::Dimensions(void) {
    return Vector(2, 2, 2);
}
Point CubeMesh::Center(void) {
    return Point(0, 0, 0);
}
int CubeMesh::FaceCount(void) {
    return sizeof(faces) / sizeof(faces[0]);
}
CubeMesh::Face CubeMesh::GetFace(int i) {
    return faces[i];
}
int CubeMesh::EdgeCount(void) {
    return sizeof(edges) / sizeof(edges[0]);
}
CubeMesh::Edge CubeMesh::GetEdge(int i) {
    return edges[i];
}

