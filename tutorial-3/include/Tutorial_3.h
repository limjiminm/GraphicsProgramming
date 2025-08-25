/*
 * Rudy Castan, Jimin Lim
 * CS200
 * Fall 2022
 */

#pragma once

#include "GLShader.h"
#include "GLVertexArray.h"
#include "IProgram.h"
#include "angles.h"
#include "color3.h"
#include "mat3.h"

#include <array>

class Tutorial_3 : public IProgram
{
public:
    /*It sets up the OpenGL viewport and specifies the initial background color.
    It creates a game object, randomly configures its model type and properties, and adds it to the list of game objects.*/
    Tutorial_3(int viewport_width, int viewport_height);

    /*Measures the current time and calculates the elapsed time in milliseconds.
    Converts the elapsed time into seconds and stores it in the deltaTime variable.
    Updates the rotation of each object in the game object list (objects) by multiplying its rotation speed with deltaTime.*/
    void Update() override;

    /*uModelToNDC is a variable representing a mat3 matrix that encompasses transformations for the model's position, rotation, and scaling. 
    It represents the transformation from model coordinate space to Normalized Device Coordinates (NDC), which is a normalized coordinate system representing graphics elements on the screen. 
    This matrix is used in the shader program to transform the vertex positions of game objects.
    It sets the drawing mode for polygons, allowing you to choose between point mode, line mode, or fill mode. This mode determines how polygons are rendered.
    Activates the shader program and calculates transformation matrices for each game object. These matrices are then passed to the shader program.
    Renders the game objects by drawing the vertices of their models on the screen.*/
    void Draw() override;

    void ImGuiDraw() override;
    void HandleEvent(SDL_Window& sdl_window, const SDL_Event& event) override;

private:
    constexpr static unsigned MAX_NUMBER_OBJECTS = 32'768;
    constexpr static float    WORLD_SIZE_MIN     = -5'000.0f;
    constexpr static float    WORLD_SIZE_MAX     = 5'000.0f;
    constexpr static float    OBJ_SIZE_MIN       = 50.0f;
    constexpr static float    OBJ_SIZE_MAX       = 400.0f;
    constexpr static float    ROT_SPEED_MIN      = util::to_radians(-30.0f);
    constexpr static float    ROT_SPEED_MAX      = util::to_radians(30.0f);

    struct
    {
        unsigned long long prev_time  = 0;
        unsigned long long start_time = 0;
        float              fps        = 0;
        int                count      = 0;
    } timing;

    enum PolyMode : GLenum
    {
        Point = GL_POINT,
        Line  = GL_LINE,
        Fill  = GL_FILL
    };
    struct
    {
        int      item_current_idx = 2;
        PolyMode type             = PolyMode::Fill;
    } poly_mode;

    // TODO
    enum ModelType
    {
        Rectangle,
        MysteryShape,
        Count
    };

    struct Object
    {
        float     rotation_speed = 0;
        float     rotation = 0;
        vec2      scale = vec2{ 1.0f };
        vec2      position = vec2{ 0.0f };
        ModelType which_model = ModelType::Rectangle;
    };

    std::vector<Object> objects;

    GLVertexArray models[ModelType::Count];

    GLShader        shader;

    int BoxNum = 0;
    int MysteryNum = 0;
 
private:
    void          on_window_resized(SDL_Window& sdl_window) noexcept;
    void          update_timing() noexcept;

    /*It creates a rectangle model and sets its position and color information.
    Configures the buffers and attribute layout for the rectangle model.
    It creates a MysteryShape model and sets its position and color information.
    Configures the buffers and attribute layout for the MysteryShape model.*/
    void          create_models();

    /*It doubles the current number of game objects while ensuring that it does not exceed the maximum object count defined as MAX_NUMBER_OBJECTS.
    It creates new objects, setting their model types and properties randomly, and adds them to the game object list.
    Based on the randomly chosen model type, it sets attributes such as position, size, rotation, and other properties randomly and adds them to the list.*/
    void          increase_number_of_objects();

    /*It reduces the current number of game objects by half.
    It removes the first object from the object list, and based on the removed object's model type, it decreases the counter for that specific model type.
    This process is repeated until the current object count reaches the new size (newSize), effectively removing objects from the list.*/
    void          decrease_number_of_objects();
};
