/*
 * Rudy Castan, Jimin.Lim
 * CS200
 * Fall 2022
 */

#pragma once

#include "GLShader.h"
#include "GLTexture.h"
#include "GLVertexArray.h"
#include "IProgram.h"
#include "color3.h"

class Tutorial_5 : public IProgram
{
public:
    /*This function is the constructor of the Tutorial_5 class.
    Specifies the viewport settings and initial background color for OpenGL.
    Load the shader, load the texture "duck.png".*/
    Tutorial_5(int viewport_width, int viewport_height);

    /*This function is called every frame to perform an update operation.
    Calculates the current animation time and updates the tileSize value by ease-in/ease-out calculation.
    Enable or disable blending according to settings.*/
    void Update() override;

    /*This function is called every frame to draw graphic elements.
    Use the shader and pass uniform values to the shader according to settings.
    Use textures and draw squares on the screen.*/
    void Draw() override;

    void ImGuiDraw() override;
    void HandleEvent(SDL_Window& sdl_window, const SDL_Event& event) override;

private:
    struct
    {
        unsigned long long prev_time   = 0;
        unsigned long long start_time  = 0;
        float              fps         = 0;
        float              ElapsedTime = 0;
        int                count;
    } timing;
    
    GLVertexArray fullscreen_quad;

    GLShader      shader;

    GLTexture     duck_image;

    struct Settings
    {
        int                   filtering_index            = 0;
        std::array<int, 2>    wrap_index                 = {0, 0};
        float                 tex_coord_scale            = 1.0f;
        bool                  modulate_color             = false;
        bool                  apply_procedural_texture   = false;
        float                 procedural_tile_size       = 8.0f;
        bool                  use_texture                = true;
        bool                  do_blending                = false;
        bool                  animate_procedural_texture = false;
        static constexpr auto MAX_TILE_SIZE              = 256.0f;
        static constexpr auto MIN_TILE_SIZE              = 16.0f;
    } settings;

    // TODO

private:
    void on_window_resized(SDL_Window& sdl_window) noexcept;
    void update_timing() noexcept;

    /*This function creates a square of full screen size.
    Sets the position, texture coordinates, and color of the rectangle, and initializes the buffer and property layout.*/
    void create_fullscreen_quad();
    void imgui_pick_filtering();
    void imgui_pick_wrapping(const char* label, GLTexture::Coordinate which_coord);

};
