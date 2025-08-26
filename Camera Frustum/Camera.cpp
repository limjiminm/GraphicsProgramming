/*!
@Camera.cpp
@author JiMin Lim
@course CS200
@Camera Frustum Programming Assignment
@date 14/11/2023
@brief This program sets the location, direction, screen size, etc. of the camera, and implements a method for moving and rotating.
Used to control the state of the camera at a given point in time.
*/

#include "Camera.h"
#include "Affine.h"
#include <cmath>

// Default constructor
Camera::Camera(void) :
    eye(Point()),
    right(Vector(1, 0, 0)),
    up(Vector(0, 1, 0)),
    back(Vector(0, 0, -1)),
    width(0),
    height(0),
    distance(1.0f),
    near(0.1f),
    far(10.0f) {}

// Non-default constructor
Camera::Camera(const Point& E, const Vector& look, const Vector& rel, float fov, float aspect, float near, float far)
    : eye(E), near(near), far(far)
{
    Vector N = normalize(look);
    Vector R = normalize(rel);

    back = -N;
    right = cross(R, back);
    up = cross(back, right);

    distance = 1.0f / tan(fov / 2);
    width = aspect * distance;
    height = distance;
}

/*!
@brief Returns the current location of the camera.
@param
@return Point
*/
/*______________________________________________________________*/
Point Camera::Eye() const
{
    return eye;
}

/*!
@brief Normalizes and returns the right direction vector of the current camera.
@param
@return Vector
*/
/*______________________________________________________________*/
Vector Camera::Right() const
{
    return normalize(right);
}

/*!
@brief Normalizes and returns the top orientation vector of the current camera.
@param
@return Vector
*/
/*______________________________________________________________*/
Vector Camera::Up() const
{
    return normalize(up);
}

/*!
@brief Normalizes and returns the current camera's rear orientation vector.
@param
@return Vector
*/
/*______________________________________________________________*/
Vector Camera::Back() const
{
    return normalize(back);
}

/*!
@brief Returns a vector representing the size and distance of the viewport on the current camera.
@param
@return Vector
*/
/*______________________________________________________________*/
Vector Camera::ViewportGeometry() const
{
    return Vector(width, height, distance);
}

/*!
@brief Returns the distance to the current camera's close plane.
@param
@return float
*/
/*______________________________________________________________*/
float Camera::NearDistance() const
{
    return near;
}

/*!
@brief Returns the distance to the far plane of the current camera.
@param
@return float
*/
/*______________________________________________________________*/
float Camera::FarDistance() const
{
    return far;
}

/*!
@brief Adjusts the screen size of the current camera by a given factor and returns the modified camera object.
@param float factor
@return Camera&
*/
/*______________________________________________________________*/
Camera& Camera::Zoom(float factor)
{
    width *= factor;
    height *= factor;
    return *this;
}

/*!
@brief Moves the current camera back and returns the modified camera object.
@param float distance
@return Camera&
*/
/*______________________________________________________________*/
Camera& Camera::Forward(float distance_increment) 
{
    eye = eye + distance_increment * back; 
    return *this;
}

/*!
@brief Rotates the camera from side to side by a given angle and returns the modified camera object.
@param float angle
@return Camera&
*/
/*______________________________________________________________*/
Camera& Camera::Yaw(float angle)
{
    Affine r = rotate(angle, up);
    right = Vector(r * right);
    back = Vector(r * back);
    return *this;
}

/*!
@brief Rotates the camera up and down by a given angle and returns the modified camera object.
@param float angle
@return Camera&
*/
/*______________________________________________________________*/
Camera& Camera::Pitch(float angle) {
    Affine r = rotate(angle, right);
    up = Vector(r * up);
    back = Vector(r * back);
    return *this;
}

/*!
@brief Rolls the camera by a given angle and returns the modified camera object.
@param float angle
@return Camera&
*/
/*______________________________________________________________*/
Camera& Camera::Roll(float angle)
{
    Affine r = rotate(angle, back);
    right = Vector(r * right);
    up = Vector(r * up);
    return *this;
}