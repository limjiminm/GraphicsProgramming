#include "Camera.h"

Camera::Camera(void)
{
	eye = Point(0, 0, 0);
	Vector L = Vector(0, 0, -1);
	up = Vector(0, 1, 0);
	back = (-1 / abs(L)) * L;
	right = (1 / abs(cross(L, up))) * cross(L, up);
	near = 0.1f;
	far = 10.0f;
	distance = far - near;
	width = (float)(2 * distance * std::tan(90 / 2));
	height = width;
}

Camera::Camera(const Point& E, const Vector& look, const Vector& vp,
	float fov, float aspect, float near, float far)
{
	eye = E;
	Vector L = look;
	back = -((1 / abs(L)) * L);
	right = (1 / abs(cross(L, vp))) * cross(L, vp);
	up = cross(back, right);
	this->near = near;
	this->far = far;
	distance = this->far - this->near;
	width = 2 * distance * std::tan(fov / 2);
	height = width / aspect;
}

Point Camera::Eye(void) const
{
	return eye;
}

Vector Camera::Right(void) const
{
	return right;
}

Vector Camera::Up(void) const
{
	return up;
}

Vector Camera::Back(void) const
{
	return back;
}

Vector Camera::ViewportGeometry(void) const
{
	Vector geometry = Vector(width, height, distance);
	return geometry;
}

float Camera::NearDistance(void) const
{
	return near;
}

float Camera::FarDistance(void) const
{
	return far;
}

Camera& Camera::Zoom(float factor)
{
	width = width * factor;
	height = height * factor;
	return *this;
}

Camera& Camera::Forward(float distance_increment)
{
	eye = eye + distance_increment * -back;
	return *this;
}

Camera& Camera::Yaw(float angle)
{
	Matrix rotationMatrix = rotate(angle, up);
	right = rotationMatrix * right;
	back = rotationMatrix * back;
	return *this;
}

Camera& Camera::Pitch(float angle)
{
	Matrix rotationMatrix = rotate(angle, right);
	up = rotationMatrix * up;
	back = rotationMatrix * back;
	return *this;
}

Camera& Camera::Roll(float angle)
{
	Matrix rotationMatrix = rotate(angle, back);
	up = rotationMatrix * up;
	right = rotationMatrix * right;
	return *this;
}