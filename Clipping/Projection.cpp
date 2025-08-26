/*!
@file Projection.cpp
@author JiMin Lim
@course CS200
@tutorial Transform Projections Programming Assignment
@date 11/19/2023
@brief This Code defines functions for generating transformation matrices to convert between camera and world coordinates, and for deriving a matrix to map camera space to normalized device coordinates (NDC).
*/

#include "Projection.h"

/*!
@brief This function constructs an affine transformation matrix mapping camera to world coordinates based on the provided Camera object's orientation and position.
@param const Camera &cam
@return Affine
//__*/
Affine CameraToWorld(const Camera& cam)
{
    Affine Val;
    Val.row[0] = Hcoord(cam.Right().x, cam.Up().x, cam.Back().x, cam.Eye().x);
    Val.row[1] = Hcoord(cam.Right().y, cam.Up().y, cam.Back().y, cam.Eye().y);
    Val.row[2] = Hcoord(cam.Right().z, cam.Up().z, cam.Back().z, cam.Eye().z);
    Val.row[3] = Hcoord(0, 0, 0, 1);
    return Val;
}

Affine WorldToCamera(const Camera& cam)
{
    Affine wortocam;

    Matrix seperM1 = Matrix{
        cam.Right(),
        cam.Up(),
        cam.Back(),
        Hcoord(0, 0, 0, 1)
    };

    Matrix seperM2 = Matrix{
        Hcoord(1, 0, 0, -cam.Eye().x),
        Hcoord(0, 1, 0, -cam.Eye().y),
        Hcoord(0, 0, 1, -cam.Eye().z),
        Hcoord(0, 0, 0, 1)
    };

    wortocam = seperM1 * seperM2;
    return wortocam;
}


/*!
@brief This function generates a matrix for transforming camera space to normalized device coordinates (NDC) using the Camera object's viewport dimensions, near and far distances, and the distance from the center of projection to the viewport.
@param const Camera &cam
@return Matrix
//__*/
Matrix CameraToNDC(const Camera& cam) {
    Vector viewportGeometry = cam.ViewportGeometry();
    float nearDistance = cam.NearDistance();
    float farDistance = cam.FarDistance();
    float distanceToViewport = viewportGeometry.z;

    Matrix ndcMatrix;
    ndcMatrix.row[0] = Hcoord((2 * distanceToViewport) / viewportGeometry.x, 0, 0, 0);
    ndcMatrix.row[1] = Hcoord(0, (2 * distanceToViewport / viewportGeometry.y), 0, 0);
    ndcMatrix.row[2] = Hcoord(0, 0, (farDistance + nearDistance) / (nearDistance - farDistance), 2 * farDistance * nearDistance / (nearDistance - farDistance));
    ndcMatrix.row[3] = Hcoord(0, 0, -1, 0);

    return ndcMatrix;
}