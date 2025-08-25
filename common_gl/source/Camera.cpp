/*
 * Jimin Lim
 * CS200
 * Fall 2023
 */
#include"Camera.h"
#include<math.h>

Camera::Camera(vec2 camera_position, vec2 camera_up) noexcept
{
	Position = camera_position;
	IsFirstPerson = false;
	up = camera_up;
	right = perpendicular_to(up); //rotate_90_degrees
	orientation = (float)atan2(right.x, right.y); //angle_representation_of_vector
}

void Camera::SetUp(vec2 camera_up) noexcept
{
	up = camera_up;
	right = perpendicular_to(up);
	orientation = (float)atan2(right.x, right.y);
}

void Camera::SetOrientation(float angle_radians) noexcept
{
	//util::to_radians(GetOrientation());
	orientation = angle_radians;
	up = { static_cast<float>(cos(orientation)),static_cast<float>(sin(orientation)) };
	right = perpendicular_to(up);
}

void Camera::MoveUp(float distance) noexcept
{
	Position = Position + up * distance;
}

void Camera::MoveRight(float distance) noexcept
{
	Position = Position + right * distance;
}

void Camera::Rotate(float angle_radians) noexcept
{
	SetOrientation(orientation + angle_radians);
}

mat3 Camera::BuildCameraToWorld() const noexcept
{
	if (IsFirstPerson)
	{
		mat3 firstPerson // build and return camera to world matrix using our up, right and position vectors
		{
			right.x, right.y,0,
			up.x, up.y,0,
			Position.x,Position.y,1
		};

		return firstPerson;
	}
	else // build and return camera to world matrix using only our position vector
	{
		mat3 NON_FirstPerson;
		return NON_FirstPerson.build_translation(Position);
	}
}

mat3 Camera::BuildWorldToCamera() const noexcept
{
	if (IsFirstPerson)
	{
		vec2 _right(-right.x, -right.y);
		vec2 _up(-up.x, -up.y);

		mat3 firstPerson
		{
			right.x	,up.x,0,
			right.y	,up.y,0,
			dot(_right,Position),dot(_up,Position), 1
		};

		return firstPerson;
	}
	else
	{
		mat3 NON_FirstPerson
		{
			1,0,0,
			0,1,0,
			-Position.x,-Position.y,1
		};

		return NON_FirstPerson;
	}
}

