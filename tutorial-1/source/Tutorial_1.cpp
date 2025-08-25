/*
 * Rudy Castan
 * CS200
 * Fall 2022
 */

#include "Tutorial_1.h"

#include "color3.h"
#include "glCheck.h"
#include "vec2.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <array>
#include <imgui.h>
#include <iostream>

util::owner<IProgram*> create_program(int viewport_width, int viewport_height)
{
    return new Tutorial_1(viewport_width, viewport_height);
}

Tutorial_1::Tutorial_1(int viewport_width, int viewport_height)
{
    // TODO
    timing.start_time = SDL_GetTicks64();
    timing.prev_time = timing.start_time;
    // Use entire window as viewport ...
    glCheck(glViewport(0, 0, viewport_width, viewport_height));
    // clear colorbuffer with RGBA value for Cornflower Blue in glClearColor ...
    glCheck(glClearColor(0.392f, 0.584f, 0.929f, 1));

    print_and_save_opengl_settings();

    shader = GLShader("Basic Shader",
        { {GLShader::VERTEX, "assets/shaders/pass_thru_pos2d_clr.vert"},
        {GLShader::FRAGMENT, "assets/shaders/basic_vtx_clr_attribute.frag"} });

    if (const bool model_and_shader_can_work = shader.IsValidWithVertexArrayObject(left_eye_model.GetHandle()); !model_and_shader_can_work)
        throw std::runtime_error{"Shader and left eye model will not work together!"};

    create_1buffer_struct_of_arrays_style();
    create_1buffer_array_of_structs_style();
    create_parallel_buffers_style();
}

void Tutorial_1::Update()
{
    update_timing();

    // 시작 색상 정의
    vec3 start_color_red(1.0f, 0.0f, 0.0f);
    vec3 start_color_blue(0.0f, 0.0f, 1.0f);
    vec3 start_color_green(0.0f, 1.0f, 0.0f);

    // 색상 전환에 사용할 시간 간격 설정 (예: 15초, 각각 5초씩)
    double transition_duration = 15.0; // 초 단위

    // 현재 시간 계산 (밀리초를 초로 변환)
    double current_time = timing.prev_time / 1000.0;

    // 색상을 부드럽게 전환 (선형 보간 사용)
    float t = static_cast<float>(fmod(current_time, transition_duration) / transition_duration); // 0부터 1까지의 보간 계수

    vec3 interpolated_color;

    if (t < 1.0f / 3.0f)
    {
        // 빨간색에서 파란색으로 전환
        t = t * 3.0f; // t를 0에서 1로 변환
        interpolated_color.red = start_color_red.red * (1.0f - t) + start_color_blue.red * t;
        interpolated_color.green = start_color_red.green * (1.0f - t) + start_color_blue.green * t;
        interpolated_color.blue = start_color_red.blue * (1.0f - t) + start_color_blue.blue * t;
    }
    else if (t < 2.0f / 3.0f)
    {
        // 파란색에서 초록색으로 전환
        t = (t - 1.0f / 3.0f) * 3.0f; // t를 0에서 1로 변환
        interpolated_color.red = start_color_blue.red * (1.0f - t) + start_color_green.red * t;
        interpolated_color.green = start_color_blue.green * (1.0f - t) + start_color_green.green * t;
        interpolated_color.blue = start_color_blue.blue * (1.0f - t) + start_color_green.blue * t;
    }
    else
    {
        // 초록색에서 빨간색으로 전환
        t = (t - 2.0f / 3.0f) * 3.0f; // t를 0에서 1로 변환
        interpolated_color.red = start_color_green.red * (1.0f - t) + start_color_red.red * t;
        interpolated_color.green = start_color_green.green * (1.0f - t) + start_color_red.green * t;
        interpolated_color.blue = start_color_green.blue * (1.0f - t) + start_color_red.blue * t;
    }

    // background 색상 업데이트
    background = interpolated_color;

    // 색상을 OpenGL에 설정하여 배경 색상 변경
    glCheck(glClearColor(background.red, background.green, background.blue, 1.0f));
}


void Tutorial_1::Draw()
{
    //glCheck(glClear(GL_COLOR_BUFFER_BIT));

    //set_clear_color(); // From Task 2

    // clear back buffer as before
    glCheck(glClear(GL_COLOR_BUFFER_BIT));

    // there are many shader programs initialized - here we're saying
    // which specific shader program should be used to render geometry

    shader.Use();
    // there are many models, each with their own initialized VAO object
    // here, we're saying which VAO's state should be used to set up pipe
    left_eye_model.Use();

    // Here we are saying to draw the model using an index buffer to describe
    // the topology
    GLDrawIndexed(left_eye_model);

    // after completing the rendering, we tell the driver that VAO
    // vaoid and current shader program are no longer current
    left_eye_model.Use(false);

    right_eye_model.Use();
    GLDrawIndexed(right_eye_model);
    right_eye_model.Use(false);

    mouth_model.Use();
    GLDrawIndexed(mouth_model);
    mouth_model.Use(false);

    shader.Use(false);

}

void Tutorial_1::ImGuiDraw()
{
    ImGui::Begin("Program Info");
    {
        ImGui::LabelText("FPS", "%.1f", timing.fps);

        for (const auto& [label, description] : settings_descriptions)
        {
            ImGui::LabelText(label, "%s", description.c_str());
        }
        {
            ImGui::LabelText("Background Color", "RGB(%.0f,%.0f,%.0f)", background.red * 255, background.green * 255, background.blue * 255);
        }
    }
    ImGui::End();
}

void Tutorial_1::HandleEvent(SDL_Window& sdl_window, const SDL_Event& event)
{
    switch (event.type)
    {
    case SDL_WINDOWEVENT:
    {
        switch (event.window.event)
        {
        case SDL_WINDOWEVENT_RESIZED:
        {
            on_window_resized(sdl_window);
        }
        break;
        case SDL_WINDOWEVENT_SIZE_CHANGED:
        {
            on_window_resized(sdl_window);
        }
        break;
        }
    }
    }
}

void Tutorial_1::create_1buffer_struct_of_arrays_style() //GPU에 기하 정보를 저장하는 방법 중 SOA방식
{
    constexpr std::array positions = { vec2{-0.2f, 0.2f}, vec2{-0.2f, 0.6f}, vec2{-0.6f, 0.6f}, vec2{-0.6f, 0.2f} };
    constexpr auto       positions_byte_size = static_cast<long long>(sizeof(vec2) * positions.size());
    constexpr std::array colors = { color3{1, 1, 1}, color3{1, 0, 0}, color3{0, 1, 0}, color3{0, 0, 1} };
    constexpr auto       colors_byte_size = static_cast<long long>(sizeof(color3) * colors.size());
    constexpr auto       buffer_size = positions_byte_size + colors_byte_size;

    GLVertexBuffer       buffer(buffer_size);
    buffer.SetData(std::span(positions));
    buffer.SetData(std::span(colors), positions_byte_size);

    GLAttributeLayout position;
    position.component_type = GLAttributeLayout::Float;
    position.component_dimension = GLAttributeLayout::_2;
    position.normalized = false;
    position.vertex_layout_location = 0;
    position.stride = sizeof(vec2);
    position.offset = 0;
    position.relative_offset = 0;

    GLAttributeLayout color;
    color.component_type = GLAttributeLayout::Float;
    color.component_dimension = GLAttributeLayout::_3;
    color.normalized = false;
    color.vertex_layout_location = 1;
    color.stride = sizeof(color3);
    color.offset = positions_byte_size;
    color.relative_offset = 0;

    left_eye_model.AddVertexBuffer(std::move(buffer), { position, color });

    left_eye_model.SetPrimitivePattern(GLPrimitive::Triangles);

    constexpr std::array<unsigned, 6> indices = { 0, 1, 2, 2, 3, 0 };
    GLIndexBuffer                     index_buffer(indices);
    left_eye_model.SetIndexBuffer(std::move(index_buffer));
}

namespace
{
    struct Vertex
    {
        vec2   position{};
        color3 color{};
    };

}

void Tutorial_1::create_1buffer_array_of_structs_style() //GPU에 기하 정보를 저장하는 방법 중 AOS방식
{
    constexpr std::array vertices = { Vertex{vec2{0.6f, 0.2f}, color3{1, 0, 0}}, Vertex{vec2{0.6f, 0.6f}, color3{0, 1, 0}}, Vertex{vec2{0.2f, 0.6f}, color3{0, 0, 1}},
                                     Vertex{vec2{0.2f, 0.2f}, color3{1, 1, 1}} };
    constexpr std::array<unsigned short, 6> indices = { 0, 1, 2, 2, 3, 0 };

    GLVertexBuffer buffer(std::span{vertices});

    // 정점 레이아웃 설정
    GLAttributeLayout position_layout;
    position_layout.component_type = GLAttributeLayout::Float;
    position_layout.component_dimension = GLAttributeLayout::_2;
    position_layout.normalized = false;
    position_layout.vertex_layout_location = 0;
    position_layout.stride = sizeof(Vertex);
    position_layout.offset = 0; 
    position_layout.relative_offset = offsetof(Vertex, position); 

    GLAttributeLayout color_layout;
    color_layout.component_type = GLAttributeLayout::Float;
    color_layout.component_dimension = GLAttributeLayout::_3;
    color_layout.normalized = false;
    color_layout.vertex_layout_location = 1;
    color_layout.stride = sizeof(Vertex);
    color_layout.offset = 0; 
    color_layout.relative_offset = offsetof(Vertex, color); 

    // 정점 배열 객체 생성
    right_eye_model.AddVertexBuffer(GLVertexBuffer(std::span{vertices}), { position_layout, color_layout });
    right_eye_model.SetIndexBuffer(GLIndexBuffer(std::span{indices}));
}

void Tutorial_1::create_parallel_buffers_style()
{
    constexpr std::array positions = { vec2{0.6f, -0.6f}, vec2{0.6f, -0.2f}, vec2{-0.6f, -0.2f}, vec2{-0.6f, -0.6f} };
    constexpr std::array colors = { color3{0, 0, 1}, color3{1, 1, 1}, color3{1, 0, 0}, color3{0, 1, 0} };
    constexpr std::array<unsigned char, 6> indices = { 0, 1, 2, 2, 3, 0 };

    // TODO
    // 위치 데이터로 사용할 버퍼 생성
    GLVertexBuffer position_buffer(std::span{positions});

    // 위치 레이아웃 정의
    GLAttributeLayout position_layout;
    position_layout.component_type = GLAttributeLayout::Float;
    position_layout.component_dimension = GLAttributeLayout::_2;
    position_layout.normalized = false;
    position_layout.vertex_layout_location = 0;
    position_layout.stride = sizeof(vec2);
    position_layout.offset = 0;
    position_layout.relative_offset = 0;

    // 색상 데이터로 사용할 버퍼 생성
    GLVertexBuffer color_buffer(std::span{colors});

    // 색상 레이아웃 정의
    GLAttributeLayout color_layout;
    color_layout.component_type = GLAttributeLayout::Float;
    color_layout.component_dimension = GLAttributeLayout::_3;
    color_layout.normalized = false;
    color_layout.vertex_layout_location = 1;
    color_layout.stride = sizeof(color3);
    color_layout.offset = 0;
    color_layout.relative_offset = 0;

    // mouth_model에 위치와 색상 버퍼 추가
    mouth_model.AddVertexBuffer(std::move(position_buffer), { position_layout });
    mouth_model.AddVertexBuffer(std::move(color_buffer), { color_layout });

    // 인덱스 버퍼 설정
    GLIndexBuffer index_buffer(std::span{indices});
    mouth_model.SetIndexBuffer(std::move(index_buffer));

    // primitive 패턴 설정
    mouth_model.SetPrimitivePattern(GLPrimitive::Triangles);
}

void Tutorial_1::on_window_resized(SDL_Window& sdl_window) noexcept
{
    int width = 0, height = 0;
    SDL_GL_GetDrawableSize(&sdl_window, &width, &height);
    glCheck(glViewport(0, 0, width, height));
}

void Tutorial_1::update_timing() noexcept
{
    const auto current_time = SDL_GetTicks64();

    timing.prev_time = current_time;
    const auto elapsed_ms = current_time - timing.start_time;

    timing.count++;
    if (elapsed_ms > 1'000)
    {
        timing.fps = timing.count / (elapsed_ms / 1'000.0);
        timing.start_time = current_time;
        timing.count = 0;
    }
}

void Tutorial_1::print_and_save_opengl_settings() noexcept
{
    //glCheck(GLubyte const* str_ven = glGetString(GL_VENDOR));
    const char* str_ven = (const char*)(glGetString(GL_VENDOR));
    settings_descriptions["GL_VENDOR"] = str_ven;

    //glCheck(GLubyte const* str_ren = (const char*)(glGetString(GL_RENDERER));
    const char* str_ren = (const char*)(glGetString(GL_RENDERER));
    settings_descriptions["GL_RENDERER"] = str_ren;

    const char* str_ver = (const char*)(glGetString(GL_VERSION));
    settings_descriptions["GL_VERSION"] = str_ver;

    const char* str_lang = (const char*)(glGetString(GL_SHADING_LANGUAGE_VERSION));
    settings_descriptions["GL_SHADING_LANGUAGE_VERSION"] = str_lang;

    GLint major_size;
    glCheck(glGetIntegerv(GL_MAJOR_VERSION, &major_size));
    settings_descriptions["GL_MAJOR_VERSION"] = std::to_string(major_size);

    GLint minor_size;
    glCheck(glGetIntegerv(GL_MINOR_VERSION, &minor_size));
    settings_descriptions["GL_MINOR_VERSION"] = std::to_string(minor_size);

    GLint vertices_size;
    glCheck(glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &vertices_size));
    settings_descriptions["GL_MAX_ELEMENTS_VERTICES"] = std::to_string(vertices_size);

    GLint indices_size;
    glCheck(glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &indices_size));
    settings_descriptions["GL_MAX_ELEMENTS_INDICES"] = std::to_string(indices_size);

    GLint image_size;
    glCheck(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &image_size));
    settings_descriptions["GL_MAX_TEXTURE_IMAGE_UNITS"] = std::to_string(image_size);

    GLint texture_size;
    glCheck(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texture_size));
    settings_descriptions["GL_MAX_TEXTURE_SIZE"] = std::to_string(texture_size);

    //GLint tex_size;
    //glCheck(glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &tex_size));
    GLint tex_size[2];
    glCheck(glGetIntegerv(GL_MAX_VIEWPORT_DIMS, tex_size));
    std::string maxView = std::to_string(tex_size[0]) + " x " + std::to_string(tex_size[1]);
    settings_descriptions["GL_MAX_VIEWPORT_DIMS"] = maxView;

    GLboolean doublebuffer;
    glCheck(glGetBooleanv(GL_DOUBLEBUFFER, &doublebuffer));

    if (doublebuffer == 1)
    {
        settings_descriptions["GL_DOUBLEBUFFER"] = "true";
    }

    if (doublebuffer == 0)
    {
        settings_descriptions["GL_DOUBLEBUFFER"] = "false";
    }

}
