/*
 * Rudy Castan
 * CS200
 * Fall 2022
 */
#pragma once

#include "mat3.h"
#include "vec2.h"


class [[nodiscard]] Camera
{
public:
    vec2 Position{};
    bool IsFirstPerson{false};

public:
    Camera() noexcept = default;

    /*Set the camera position to the given camera_position.
    Set camera mode to 1 person "First Person" mode.
    Set the camera's "up" direction (up) to the given camera_up, calculate the "right" direction (right) accordingly, and set the camera's direction using atan2.*/
    Camera(vec2 camera_position, vec2 camera_up) noexcept;


    [[nodiscard]] vec2 GetUp() const noexcept
    {
        return up;
    }

    [[nodiscard]] vec2 GetRight() const noexcept
    {
        return right;
    }

    [[nodiscard]] float GetOrientation() const noexcept
    {
        return orientation;
    }

    /*Set the "up" direction vector (up) to the given camera_up vector.
    Calculate the "right" direction vector (right) using the permanent_to function and set the direction using the atan2 function.*/
    void SetUp(vec2 camera_up = {0, 1}) noexcept;

    /*Set the direction of the camera to a given angle (radian).
    Update the "up" direction vector (up) and the "right" direction vector (right) according to the given angle.*/
    void SetOrientation(float angle_radians) noexcept;

    /*Move the camera by a given distance in the current "up" direction.
    Update the position by adding the result obtained by multiplying the position vector (up) by distance.*/
    void MoveUp(float distance) noexcept;

    /*Move the camera by a given distance in the current "right" direction.
    Update the position by adding the result of multiplying the position vector by the "right" direction vector (right) by distance.
    */
    void MoveRight(float distance) noexcept;

    /*Turn the camera's orientation by adding a given angle (radian) to the current orientation.
    Set orientation by invoking the Set Orientation function*/
    void Rotate(float angle_radians) noexcept;

    /*Creates a matrix that converts from camera coordinates to world coordinates.
    If IsFirstPerson is true, construct and return the matrix using the camera position and the "up" and "right" directions.
    If IsFirstPerson is false, it generates and returns a movement matrix considering only the camera location.*/
    mat3 BuildCameraToWorld() const noexcept;

    /*Creates a matrix that converts from world coordinates to camera coordinates.
    If IsFirstPerson is true, it generates and returns a reverse transformation matrix.
    If IsFirstPerson is false, it generates and returns a moving matrix that transforms world coordinates into camera coordinates.*/
    mat3 BuildWorldToCamera() const noexcept;

private:
    vec2  up{0, 1};
    vec2  right{1, 0};
    float orientation{0};
};
