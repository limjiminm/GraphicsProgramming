/*
 * Rudy Castan / Jimin Lim
 * CS200
 * Fall 2022
 */

#pragma once

#include "GLShader.h"
#include "GLVertexArray.h"
#include "IProgram.h"

class Tutorial_2 : public IProgram
{
public:

    //Sets the viewport size and initializes viewports.
    //Creates a shader for the models.
    //Builds models for points, lines, triangle fans, and triangle strips.
    Tutorial_2(int viewport_width, int viewport_height);

    void Update() override;

    //Utilizes shaders to render graphical elements. Renders distinct geometric elements in four separate screen viewports.
    void Draw() override;
    void ImGuiDraw() override;
    void HandleEvent(SDL_Window& sdl_window, const SDL_Event& event) override;

private:

    struct
    {
        unsigned long long prev_time  = 0;
        unsigned long long start_time = 0;
        float              fps        = 0;
        int                count      = 0;
    } timing;

    enum ViewportPosition {
        TopLeft = 0,
        TopRight = 1,
        BottomLeft = 2,
        BottomRight = 3
    };

    struct {
        int x = 0, y = 0, width = 0, height = 0;
    } viewports[4];

private:
    void on_window_resized(SDL_Window& sdl_window) noexcept;
    void update_timing() noexcept;

    //This function calculates and sets up four viewports based on the provided screen width and height.
    void set_viewports(int width, int height);

    //This function generates points and sets up a model to represent these points. 
    //The model is responsible for displaying points in the top-left corner of the screen.
    void create_points_model();

    //This function generates lines and sets up a model to represent these lines. The model is responsible for displaying lines in the top-right corner of the screen.
    void create_lines_model();

    //This function creates and configures a model that draws circles with random colors. The model is responsible for rendering these circles in the bottom-left corner of the screen.
    void create_trifan_model();

    //This function creates and configures a model to draw a triangle strip in a grid pattern in the bottom-right corner of the screen. 
    //It calculates the positions of each vertex and assigns them random colors.
    void create_tristrip_model();

    GLShader        shader;
    GLVertexArray   models[4];
};
