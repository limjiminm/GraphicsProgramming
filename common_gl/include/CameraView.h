/*
 * Rudy Castan
 * CS200
 * Fall 2022
 */
#pragma once
#include "mat3.h"
#include "vec2.h"

class [[nodiscard]] CameraView
{
public:
    //Sets the frame buffer size to the_framebuffer_size given.
    void SetFramebufferSize(vec2 the_framebuffer_size) noexcept;

    //Sets the horizontal and vertical sizes of the frame buffer to the given values.
    void SetFramebufferSize(int new_framebuffer_width, int new_framebuffer_height) noexcept;

    /*Returns the size of the current camera view
    The returned size is represented in the form of vec2, a 2D vector, and is mainly used to represent the viewport or frame buffer size.*/
    vec2 GetViewSize() const;

    //Sets the zoom value to the given value, new_zoom.
    void SetZoom(float new_zoom) noexcept;

    vec2 CalcViewSizeWithZoom() const noexcept
    {
        return framebuffer_size / zoom;
    }

    [[nodiscard]] float GetZoom() const noexcept
    {
        return zoom;
    }

    /*Creates and returns a matrix that converts from camera coordinates to normalizer coordinates (NDC). 
    This matrix takes into account the size of the view and the zoom value.*/
    mat3 BuildCameraToNDC() const noexcept;

    /*Creates and returns a matrix that converts from window device coordinates to camera coordinates. 
    This matrix takes into account the size of the view and the zoom value.*/
    mat3 BuildWindowDeviceToCamera() const noexcept;

private:
    vec2  framebuffer_size{};
    float zoom{1.0f};
};
