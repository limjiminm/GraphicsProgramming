/*
 * Rudy Castan , Ji Min LIm
 * CS200
 * Fall 2022
 */

#include "Tutorial_4.h"

#include "angles.h"
#include "color3.h"
#include "glCheck.h"
#include "vec2.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <algorithm>
#include <array>
#include <fstream>
#include <imgui.h>
#include <iostream>
#include <sstream>

util::owner<IProgram *> create_program(int viewport_width, int viewport_height)
{
    return new Tutorial_4(viewport_width, viewport_height);
}

Tutorial_4::Tutorial_4(int viewport_width, int viewport_height)
{
    // TODO
    caminfo.camera_view.SetFramebufferSize(viewport_width, viewport_height); // Camera → NDC matrix transformation
    glCheck(glClearColor(0.392f, 0.584f, 0.929f, 1));

    load_scene("assets/scenes/tutorial-4.scn");

    timing.start_time = SDL_GetTicks64();
    timing.prev_time = timing.start_time;
}

void Tutorial_4::Update()
{
    update_timing();

    // TODO

    // Update Camera:
    caminfo.camera.MoveUp(caminfo.move_speed * timing.DeltaTime * caminfo.move_scalar);
    caminfo.camera.MoveRight(caminfo.move_speed * timing.DeltaTime * caminfo.strafe_scalar);
    caminfo.camera.Rotate(util::to_radians(caminfo.obj.angle_speed) * timing.DeltaTime * caminfo.turn_scalar);

    //Transform from world coordinate to camera coordinate system
    // Information such as where the camera is located, which direction it is facing, and from what angle it is looking at the world
    mat3 world_to_cam = caminfo.camera.BuildWorldToCamera();

    //move from camera coordinate to ndc coordinate system
    //Decide how an object or background in the game appears on the screen
    mat3 cam_to_ndc = caminfo.camera_view.BuildCameraToNDC();

    //convert points in the world coordinate system to points in the ndc coordinate system at once
    //How objects or backgrounds in the game appear on the screen
    mat3 world_to_ndc = cam_to_ndc * world_to_cam;

    // Iterate over each game object
    for (Object& obj : objects)
    {
        //Calculate the angle of how much an object needs to turn--> Fast rotation
        obj.angle_disp += util::to_radians(obj.angle_speed) + timing.DeltaTime;

        //calculate where the item is located
        mat3 T = mat3::build_translation(obj.position);

        //calculate how an object rotates
        mat3 R = mat3::build_rotation(util::to_radians(obj.angle_disp));

        //calculate the size of the object
        mat3 S = mat3::build_scale(obj.scaling);

        //mat3 T = mat3::build_translation(obj.position);
        //mat3 R = mat3::build_rotation(obj.angle_disp);
        //mat3 R = mat3::build_rotation(utill::to_radians(obj.angle_disp));

        //coordinates that make things visible in the game
        mat3 model_to_word = T * R * S;

        //Decide how to draw on the actual screen
        obj.model_to_ndc = world_to_ndc * model_to_word;
    }

    //Indicates exactly where the camera is located in the game world
    mat3 Camera_T = mat3::build_translation(caminfo.camera.Position);

    //Indicates which direction the camera is facing or how much has been rotated
    mat3 Camera_R = mat3::build_rotation(caminfo.camera.GetOrientation());

    //How large or reduced the camera's size
    mat3 Camera_S = mat3::build_scale(caminfo.obj.scaling);

    caminfo.obj.model_to_ndc = world_to_ndc * Camera_T * Camera_R * Camera_S;

    compute_mouse_coordinates();
}


namespace
{
    std::span<const float, 3 * 3> to_span(const mat3 &m)
    {
        return std::span<const float, 3 * 3>(&m.elements[0][0], 9);
    }
    std::span<const float, 3> to_span(const color3 &c)
    {
        return std::span<const float, 3>(&c.elements[0], 3);
    }
}

void Tutorial_4::Draw()
{
    // Clear the color buffer
    glCheck(glClearColor(0.392f, 0.584f, 0.929f, 1));
    glCheck(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)); // Clear both color and depth buffer

    if (!objects.empty())
    {
        unsigned current_shader = 0;
        unsigned current_model = 0;
        //unsigned current_model_cam = 0;
        //unsigned current_shader_cam = 0;

        shaders.at(0).Use();
        models.at(0).Use();

        for (const Object& obj : objects)//객체마다 다른 셰이더를 사용할 수가 있음
        {
            // Check if object's shader doesn't match the current one
            //어떤 객체가 어떤 셰이더를 사용하는지 파악
            if (obj.shader_index != current_shader)//다른 셰이더를 사용한다는 뜻
            {
                current_shader = obj.shader_index;
                shaders.at(current_shader).Use(); // Use the object's shader
                //shaders[current_shader].Use();
            }

            // Check if object's model doesn't match the current one
            if (obj.model_index != current_model)//다른 모델을 사용함
            {
                current_model = obj.model_index;
                //models[current_model].Use();
                models.at(current_model).Use(); // Use the object's model
            }

            shaders[current_shader].SendUniform("uModelToNDC", to_span(obj.model_to_ndc));
            shaders[current_shader].SendUniform("uFillColor", to_span(obj.color));

            //shaders.at(current_shader).SendUniform("uModelToNDC", to_span(obj.model_to_ndc));
            //shaders.at(current_shader).SendUniform("uFillColor", to_span(obj.color));

            // Draw the current indexed model
            //glDrawElements(GL_TRIANGLES, models.at(current_model).GetIndicesCount(),GL_UNSIGNED_SHORT, nullptr);
            glDrawElements(models.at(current_model).GetPrimitivePattern(), models.at(current_model).GetIndicesCount(), GL_UNSIGNED_SHORT, nullptr);

        }

        current_shader = caminfo.obj.shader_index;
        shaders.at(current_shader).Use();

        current_model = caminfo.obj.model_index;
        models.at(current_model).Use();
        
        shaders[current_shader].SendUniform("uModelToNDC", to_span(caminfo.obj.model_to_ndc));
        shaders[current_shader].SendUniform("uFillColor", to_span(caminfo.obj.color));

        glDrawElements(models.at(caminfo.obj.model_index).GetPrimitivePattern(), models.at(caminfo.obj.model_index).GetIndicesCount(), GL_UNSIGNED_SHORT, nullptr);

    }

}


void Tutorial_4::ImGuiDraw()
{
    ImGui::Begin("Program Info");
    {
        ImGui::LabelText("FPS", "%.1f", static_cast<double>(timing.fps));
        ImGui::LabelText("Delta time", "%.3f seconds", static_cast<double>(timing.DeltaTime));
        ImGui::LabelText("Mouse Positions", "Device(%4.0f,%4.0f)\nCamera(%4.0f,%4.0f)\nWorld (%4.0f,%4.0f)", static_cast<double>(environment.mouseX),
                         static_cast<double>(environment.mouseY),                                                             // device space
                         static_cast<double>(environment.mouseCamSpaceX), static_cast<double>(environment.mouseCamSpaceY),    // in Camera Space
                         static_cast<double>(environment.mouseWorldSpaceX), static_cast<double>(environment.mouseWorldSpaceY) // in World Space
        );
        const auto cam_pos = caminfo.camera.Position;
        ImGui::LabelText("Camera World Position", "(%.1f,%.1f)", static_cast<double>(cam_pos.x), static_cast<double>(cam_pos.y));
        const auto cam_orientation = int(util::to_degrees(caminfo.camera.GetOrientation()) + 360) % 360;
        ImGui::LabelText("Camera World Orientation", "%d degrees", cam_orientation);
        const auto cam_size = caminfo.camera_view.CalcViewSizeWithZoom();
        ImGui::LabelText("Camera View Size", "%.1f x %.1f", static_cast<double>(cam_size.width), static_cast<double>(cam_size.height));
        auto zoom = caminfo.camera_view.GetZoom() * 100.0f;
        if (ImGui::SliderFloat("Zoom", &zoom, 25.0f, 400.0f, "%.0f%%"))
        {
            caminfo.camera_view.SetZoom(zoom / 100.0f);
        }

        ImGui::Checkbox("First Person Camera", &caminfo.camera.IsFirstPerson);

        if (ImGui::Button("See Camera Controls?"))
            ImGui::OpenPopup("Camera Controls");
        const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopup("Camera Controls", ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Left/Right Arrows to turn camera left & right");
            ImGui::Separator();
            ImGui::Text("Up/Down Arrows to turn move camera forward & backward");
            ImGui::Separator();
            ImGui::Text("A/D keys to strafe left & right");
            ImGui::Separator();
            ImGui::SetItemDefaultFocus();
            if (ImGui::Button("Okay"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

void Tutorial_4::HandleEvent(SDL_Window& sdl_window, const SDL_Event& event)
{
    switch (event.type)
    {
    case SDL_MOUSEMOTION:
        // make sure we only save mouse positions for the main window and not some ImGui window
        if (event.motion.windowID == SDL_GetWindowID(&sdl_window))
        {
            // get the mouse device positions from event.motion.x/event.motion.y
            int mouseX = event.motion.x;
            int mouseY = event.motion.y;

            // Convert to float and store in environment
            environment.mouseX = static_cast<int>(mouseX);
            environment.mouseY = static_cast<int>(mouseY);
        }
        break;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
    {
        if (event.key.state == SDL_PRESSED)
        {
            // pressed -> float of 1
            float pressedState = 1.0f;
            switch (event.key.keysym.sym)
            {
            case SDLK_UP:
                caminfo.move_scalar = pressedState;
                break;
            case SDLK_DOWN:
                caminfo.move_scalar = -pressedState;
                break;
            case SDLK_LEFT:
                caminfo.turn_scalar = pressedState;
                break;
            case SDLK_RIGHT:
                caminfo.turn_scalar = -pressedState;
                break;
            case SDLK_a:
                caminfo.strafe_scalar = -pressedState;
                break;
            case SDLK_d:
                caminfo.strafe_scalar = pressedState;
                break;
            }
        }
        else
        { // SDL_RELEASED
            // release -> float of 0
            float releasedState = 0.0f;
            switch (event.key.keysym.sym)
            {
            case SDLK_UP:
            case SDLK_DOWN:
                caminfo.move_scalar = releasedState;
                break;
            case SDLK_LEFT:
            case SDLK_RIGHT:
                caminfo.turn_scalar = releasedState;
                break;
            case SDLK_a:
            case SDLK_d:
                caminfo.strafe_scalar = releasedState;
                break;
            }
        }
    }
    break;
    }
}


void Tutorial_4::on_window_resized(SDL_Window& sdl_window) noexcept
{
    int width = 0, height = 0;
    SDL_GL_GetDrawableSize(&sdl_window, &width, &height);
    glCheck(glViewport(0, 0, width, height));
    
    caminfo.camera_view.SetFramebufferSize(width, height);
}

void Tutorial_4::update_timing() noexcept
{
    const auto current_time = SDL_GetTicks64();
    const auto delta_ms = current_time - timing.prev_time;
    timing.DeltaTime = delta_ms / 1'000.0f;

    timing.prev_time = current_time;
    const auto elapsed_ms = current_time - timing.start_time;

    timing.count++;
    if (elapsed_ms > 1'000)
    {
        timing.fps = timing.count / (elapsed_ms / 1'000.0f);
        timing.start_time = current_time;
        timing.count = 0;
    }
}

void Tutorial_4::compute_mouse_coordinates() noexcept
{// Use camera view BuildWindowDeviceToCamera to convert to camera space
    mat3 CameraSpace = caminfo.camera_view.BuildWindowDeviceToCamera();
    vec3 DevicePosition(static_cast<float>(environment.mouseX), static_cast<float>(environment.mouseY), 1.0f);
    vec3 CameraSpace_Position = CameraSpace * DevicePosition;

    // Save Camera Space coordinates to environment
    float CamSpace_X = CameraSpace_Position.x;
    float CamSpace_Y = CameraSpace_Position.y;

    environment.mouseCamSpaceX = CamSpace_X;
    environment.mouseCamSpaceY = CamSpace_Y;

    // Use camera BuildCameraToWorld to convert to world space
    mat3 CameraToWorld = caminfo.camera.BuildCameraToWorld(); // Assuming BuildCameraToWorld returns a mat3
    vec3 WorldSpace_Position = CameraToWorld * CameraSpace_Position; // Transform from camera space to world space

    // Save World Space coordinates to environment
    float WorldSpace_X = WorldSpace_Position.x;
    float WorldSpace_Y = WorldSpace_Position.y;

    environment.mouseWorldSpaceX = WorldSpace_X;
    environment.mouseWorldSpaceY = WorldSpace_Y;
}

void Tutorial_4::load_scene(const std::filesystem::path &scene_file_path)
{
    std::ifstream sceneFile{scene_file_path};
    if (!sceneFile)
    {
        throw std::runtime_error(std::string("ERROR: Unable to open scene file: ") + scene_file_path.string());
    }

    std::string line;
    std::getline(sceneFile, line);

    int objectCount;
    std::istringstream{line} >> objectCount;
    objects.reserve(static_cast<unsigned>(objectCount));

    std::string           modelName;
    std::string           objectName;
    std::string           shaderProgramName;
    std::filesystem::path vertexShaderPath, fragmentShaderPath;

    for (int i = 0; i < objectCount; ++i)
    {
        Object object;
        std::getline(sceneFile, line);
        std::istringstream{line} >> modelName;

        std::getline(sceneFile, line);
        std::istringstream{line} >> object.name;

        object.model_index = load_mesh_and_get_id(std::filesystem::path("assets") / "meshes" / (modelName + ".msh"));

        std::getline(sceneFile, line);
        std::istringstream{line} >> shaderProgramName >> vertexShaderPath >> fragmentShaderPath;
        object.shader_index = load_shader_and_get_id(shaderProgramName, vertexShaderPath, fragmentShaderPath); //주소값을 읽어야함

        // Add code to read the remaining object's parameters:
        // RGB Color
        std::getline(sceneFile, line);
        std::istringstream{line} >> object.color.red >> object.color.green >> object.color.blue;
        // Width Height
        std::getline(sceneFile, line);
        std::istringstream{line} >> object.scaling.x >> object.scaling.y;
        // Orientation, Rotation Speed
        std::getline(sceneFile, line);
        std::istringstream{line} >> object.angle_disp >> object.angle_speed;
        // 2D Position
        std::getline(sceneFile, line);
        std::istringstream{line} >> object.position.x >> object.position.y;

        if (object.name != "Camera")
        {
            objects.emplace_back(std::move(object));
        }
        else
        {
            caminfo.obj = object;
            caminfo.camera.SetOrientation(object.angle_disp);
            caminfo.camera.Position = object.position;
        }
    }
}

unsigned Tutorial_4::load_mesh_and_get_id(const std::filesystem::path &mesh_file_path)
{
    // Check if the model already exists in the map
    auto it = asset_to_index.find(mesh_file_path.string());

    if (it != asset_to_index.end())
    {
        // If the model already exists, get its saved location
        unsigned savedLocation = it->second;

        // Return the saved location
        return savedLocation;
    }

    std::ifstream meshFile{mesh_file_path};
    if (!meshFile)
    {
        throw std::runtime_error{std::string("ERROR: Unable to open mesh file: ") + mesh_file_path.string()};
    }

    std::vector<vec2>           vertex_positions;
    std::vector<unsigned short> vertex_indices;

    std::string       modelName;
    float             x = 0, y = 0;
    GLushort          index = 0;
    std::string       line;
    char              mode = 0;
    GLPrimitive::Type primitive_type = GLPrimitive::Points;

    while (meshFile)
    {
        std::getline(meshFile, line);
        if (line.empty())
        {
            continue;
        }

        std::istringstream sstr{line};
        sstr >> mode;

        if (mode == 'n')
        {
            // Read modelName
            sstr >> modelName;
        }
        else if (mode == 'v')
        {
            // vertex position and add it to vertex_positions
            sstr >> x >> y;
            vertex_positions.push_back({ x, y });
        }

        else if (mode == 't' || mode == 'f')
        {
            if (mode == 't')
            {
                primitive_type = GLPrimitive::Triangles;

                // Read 3 indices for a triangle
                for (int i = 0; i < 3 && (sstr >> index); i++)
                {
                    vertex_indices.push_back(index);
                }
            }
            else if (mode == 'f')
            {
                primitive_type = GLPrimitive::TriangleFan;

                // Read indices until end of line for a triangle fan
                while (sstr >> index)
                {
                    vertex_indices.push_back(index);
                }
            }
        }


    }
    // Create GLVertexArray with saved primitive_type
    GLVertexArray modleVertex(primitive_type);

    // Set Vertex Count based off of vertex_positions size
    modleVertex.SetVertexCount(static_cast<unsigned>(vertex_positions.size()));

    // Describe the Position Layout
    GLAttributeLayout position;
    position.component_dimension = GLAttributeLayout::_2;
    position.component_type = GLAttributeLayout::Float;
    position.normalized = false;
    position.offset = 0;
    position.relative_offset = 0;
    position.stride = sizeof(vec2);
    position.vertex_layout_location = 0;

    // Create and add Vertex Buffer of positions to GLVertexArray
    GLVertexBuffer position_buffer(std::span{vertex_positions});

    // Create and add Index Buffer to GLVertexArray
    GLIndexBuffer index_buffer(std::span{vertex_indices});

    modleVertex.AddVertexBuffer(std::move(position_buffer), { position });
    modleVertex.SetIndexBuffer(std::move(index_buffer));

    // Add Vertex Array to models container and save it's index location
    models.push_back(std::move(modleVertex));

    // Save location to asset_to_index map
    unsigned location = static_cast<unsigned>(models.size()) - 1;
    asset_to_index.insert({ std::string(modelName), location });
 
    return location;
}

unsigned Tutorial_4::load_shader_and_get_id(std::string_view name, const std::filesystem::path &vertex_filepath, const std::filesystem::path &fragment_filepath)
{
    // Insert if 1st time otherwise get what is there
    auto [iter, is_new] = asset_to_index.insert({ std::string(name), 0 });
    auto& location = iter->second;
    if (!is_new)
    {
        return location;
    }
    location = static_cast<unsigned>(shaders.size());
    shaders.emplace_back(GLShader(name, { {GLShader::VERTEX, vertex_filepath}, {GLShader::FRAGMENT, fragment_filepath} }));
    return location;
}
