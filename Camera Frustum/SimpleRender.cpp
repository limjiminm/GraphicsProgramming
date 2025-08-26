/*!
@SimpleRender.cpp
@author JiMin Lim
@course CS200
@Camera Frustum Programming Assignment
@date 14/11/2023
@brief This program provides the ability to convert the edges and faces of the 3D model and render them on the screen.
In particular, projection transformation and surface shading are performed to achieve three-dimensional effects.
*/

#include <cmath>
#include "SimpleRender.h"

SimpleRender::SimpleRender(Render& r) : render(r)
{
    Matrix M;
    M.row[0] = Hcoord(1.f, 0, 0, 0);
    M.row[1] = Hcoord(0, 1.f, 0, 0);
    M.row[2] = Hcoord(0, 0, 1.f / 11, -5.f / 11);
    M.row[3] = Hcoord(0, 0, -2.f / 11, 10.f / 11);

    PersProj = M;
}


SimpleRender::~SimpleRender(void)
{

}

/*!
@brief transforms and projects the edges of the model and renders them on the screen
@param Mesh& m, const Affine& A, const Vector& color
@return void
*/
/*______________________________________________________________*/
void SimpleRender::DisplayEdges(Mesh& m, const Affine& A, const Vector& color) {
    render.SetColor(color);

    int edgeCount = m.EdgeCount();
    int i = 0;

    while (i < edgeCount) {
        const Mesh::Edge& e = m.GetEdge(i);

        // Transform vertices of the edge using the affine transformation
        Hcoord V1 = A * m.GetVertex(e.index1);
        Hcoord V2 = A * m.GetVertex(e.index2);

        // Apply perspective projection
        V1 = PersProj * V1;
        V2 = PersProj * V2;

        // Perform perspective division
        V1 = (V1.w != 0.0f) ? (1.0f / V1.w) * V1 : V1;
        V2 = (V2.w != 0.0f) ? (1.0f / V2.w) * V2 : V2;

        // Store transformed vertices in proj_vertices
        proj_vertices.clear();
        proj_vertices.push_back(Point(V1.x, V1.y, V1.z));
        proj_vertices.push_back(Point(V2.x, V2.y, V2.z));

        // Draw the transformed edge using proj_vertices
        render.DrawLine(proj_vertices[0], proj_vertices[1]);

        // Increment the loop variable
        ++i;
    }
}

/*!
@brief transforms and projects faces to display three-dimensionally shaded faces on the screen.
@param Mesh& m, const Affine& A, const Vector& color
@return void
*/
/*______________________________________________________________*/
void SimpleRender::DisplayFaces(Mesh& m, const Affine& A, const Vector& color)
{
    Point eyePosition(0, 0, 5);
    Vector lightDirection(0, 0, 1);

    int faceCount = m.FaceCount();

    for (int i = 0; i < faceCount; i++)
    {
        Mesh::Face f = m.GetFace(i);

        // Transform vertices of the face using the affine transformation
        Hcoord V1 = A * m.GetVertex(f.index1);
        Hcoord V2 = A * m.GetVertex(f.index2);
        Hcoord V3 = A * m.GetVertex(f.index3);

        Vector normal = cross(Vector(V2 - V1), Vector(V3 - V1));

        if (dot(normal, Vector(V1 - eyePosition)) > 0)
            continue;

        V1 = PersProj * V1;
        V2 = PersProj * V2;
        V3 = PersProj * V3;

        // Perform perspective division
        V1 = (V1.w != 0.0f) ? (1.0f / V1.w) * V1 : V1;
        V2 = (V2.w != 0.0f) ? (1.0f / V2.w) * V2 : V2;
        V3 = (V3.w != 0.0f) ? (1.0f / V3.w) * V3 : V3;


        normal = normalize(normal);
        float mu = std::abs(dot(lightDirection, normal));

        proj_vertices.clear();
        proj_vertices.push_back(Point(V1.x, V1.y, V1.z));
        proj_vertices.push_back(Point(V2.x, V2.y, V2.z));
        proj_vertices.push_back(Point(V3.x, V3.y, V3.z));

        Vector shadedColor = mu * color;

        render.SetColor(shadedColor);
        render.FillTriangle(proj_vertices[0], proj_vertices[1], proj_vertices[2]);
    }
}
