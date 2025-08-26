// Clip.cpp

/*!
@Clip.cpp
@author JiMin Lim
@course CS200
@Clipping Programming Assignment
@date 1/12/2023
@brief Implementation of a clipping algorithm for a set of homogeneous coordinates.
*/


#include "Clip.h"
#include "Affine.h"
#include "HalfSpace.h"

/*!
@brief Clips a set of vertices against a collection of half-spaces.
@param std::vector<Hcoord>& vertices
@return bool
*/
/*______________________________________________________________*/
bool Clip::operator()(std::vector<Hcoord>& vertices) {
    if (vertices.empty())
    {
        return false;
    }

    for (const auto& half_space : half_spaces) {
        std::vector<Hcoord> new_vertices;

        for (size_t i = 0; i < vertices.size(); ++i)
        {
            size_t next_index = (i + 1) % vertices.size();
            Hcoord p1 = vertices[i];
            Hcoord p2 = vertices[next_index];

            float dot1 = dot(half_space, p1);
            float dot2 = dot(half_space, p2);

            if (dot1 > 0 && dot2 <= 0)
            {
                float t = dot1 / (dot1 - dot2);
                Hcoord intersection;
                intersection.x = p1.x + t * (p2.x - p1.x);
                intersection.y = p1.y + t * (p2.y - p1.y);
                intersection.z = p1.z + t * (p2.z - p1.z);
                intersection.w = p1.w + t * (p2.w - p1.w);

                new_vertices.push_back(intersection);
            }
            else if (dot1 <= 0 && dot2 <= 0)
            {
                new_vertices.push_back(p1);
            }
            else if (dot1 <= 0 && dot2 > 0)
            {
                new_vertices.push_back(p1);

                float t = dot1 / (dot1 - dot2);
                Hcoord intersection;
                intersection.x = p1.x + t * (p2.x - p1.x);
                intersection.y = p1.y + t * (p2.y - p1.y);
                intersection.z = p1.z + t * (p2.z - p1.z);
                intersection.w = p1.w + t * (p2.w - p1.w);

                new_vertices.push_back(intersection);
            }
        }

        if (new_vertices.empty())
        {
            return false;
        }

        vertices = new_vertices;
    }

    return true;
}






