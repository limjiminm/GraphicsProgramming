/*
 * Rudy Castan
 * CS200
 * Fall 2022
 */

#pragma once

#include "Camera.h"
#include "CameraView.h"
#include "GLShader.h"
#include "GLVertexArray.h"
#include "IProgram.h"
#include "angles.h"
#include "color3.h"
#include "mat3.h"

#include <unordered_map>
#include <vector>

class Tutorial_4 : public IProgram
{
public:
    Tutorial_4(int viewport_width, int viewport_height);

    /*Update the camera position and orientation and generate a transformation matrix from the world to the camera to the NDC.
    Calculate the transformation matrix based on the rotation, position, and size of each game object to determine how to draw it on the screen.
    Compute mouse coordinates and create a transformation matrix based on the actual location, direction, and size of the camera*/
    void Update() override;

    /*Initialize the color and depth buffer of the screen, and set the default shader and model.
    For each game object, draw on the screen based on the object's transformation matrix and color, using the corresponding shader and model.
    Draws specific objects on the screen based on camera-related information.*/
    void Draw() override;
    void ImGuiDraw() override;

    /*When a mouse movement is detected, the location of the mouse is saved in an environment variable.
    Determines the direction of the camera's movement and rotation when a specific key on the keyboard is pressed and sets the corresponding value.
    Set the camera movement or rotation value in that direction to 0 when a specific key on the keyboard is placed.*/
    void HandleEvent(SDL_Window& sdl_window, const SDL_Event& event) override;

private:
    struct
    {
        unsigned long long prev_time  = 0;
        unsigned long long start_time = 0;
        float              fps        = 0;
        float              DeltaTime{0};
        int                count;
    } timing;

    struct
    {
        int   mouseX           = 0;
        int   mouseY           = 0;
        float mouseCamSpaceX   = 0;
        float mouseCamSpaceY   = 0;
        float mouseWorldSpaceX = 0;
        float mouseWorldSpaceY = 0;
    } environment;

    // TODO
    struct Object
    {
        unsigned    model_index = 0;
        std::string name;
        unsigned    shader_index = 0;
        color3      color{};
        vec2        scaling{ 1.0f };
        float       angle_speed = 0;
        float       angle_disp = 0;
        vec2        position{};
        mat3        model_to_ndc{};
    };

    struct CamInfo
    {
        Object     obj{};
        Camera     camera{};
        CameraView camera_view{};
        float      move_scalar = 0;
        float      turn_scalar = 0;
        float      strafe_scalar = 0;
        float      move_speed = 120.0f;
    } caminfo;

private:
    //Update viewport size for camera information
    void     on_window_resized(SDL_Window& sdl_window) noexcept;
    void     update_timing() noexcept;

    /*Convert mouse coordinates to camera space.
    Stores the camera space coordinates in an environment variable.
    Convert camera space coordinates to world space and store world space coordinates in environmental variables.*/
    void     compute_mouse_coordinates() noexcept;

    /*Opens a file in the specified path and generates an exception if the file does not exist.
    Initialize the vectors by reading the number of objects from the file.
    Read the information from the object from a file and add it to the object vector or set camera information.*/
    void     load_scene(const std::filesystem::path& scene_file_path);

    /*Read and save vertex locations and triangular indexes from the model file.
    Create a GLVertex Array for the model and set the vertex and index data.
    Add the model to the model container (models), save the model location to the asset_to_index map, and return the location.*/
    unsigned load_mesh_and_get_id(const std::filesystem::path& mesh_file_path);
    unsigned load_shader_and_get_id(std::string_view name, const std::filesystem::path& vertex_filepath, const std::filesystem::path& fragment_filepath);

    std::vector<GLShader>      shaders;
    std::vector<GLVertexArray> models;
    std::vector<Object>        objects;
    std::unordered_map<std::string, unsigned> asset_to_index;
};
