/*
 * Rudy Castan, Jimin.Lim
 * CS200
 * Fall 2022
 */

#include "Tutorial_5.h"

#include "angles.h"
#include "color3.h"
#include "glCheck.h"
#include "vec2.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <array>
#include <cmath>
#include <fstream>
#include <imgui.h>
#include <iostream>
#include <string_view>

util::owner<IProgram*> create_program(int viewport_width, int viewport_height)
{
    return new Tutorial_5(viewport_width, viewport_height);
}

//namespace
//{
//#include "tutorial-5.frag.h"
//#include "tutorial-5.vert.h"
//}

Tutorial_5::Tutorial_5(int viewport_width, int viewport_height)
{
    // TODO
    glCheck(glViewport(0, 0, viewport_width, viewport_height));
    glCheck(glClearColor(0.392f, 0.584f, 0.929f, 1));
    shader = GLShader("Basic Shader",
        { {GLShader::VERTEX, "assets/shaders/tutorial-5.vert"},
        {GLShader::FRAGMENT, "assets/shaders/tutorial-5.frag"} });

    if (const bool loaded = duck_image.LoadFromFileImage("assets/images/duck.png"); !loaded)
    {
        throw std::runtime_error{"Failed to load the duck.png"};
    }

    timing.start_time = SDL_GetTicks64();
    timing.prev_time  = timing.start_time;
    create_fullscreen_quad();
}

void Tutorial_5::Update()
{
    update_timing();
    const auto animationTime = timing.ElapsedTime;// 현재 경과 시간

    float T = 30.0f; // 전체 애니메이션 시간
    float t = animationTime / T; // [0, 1] 범위
   
    const auto e = (sin(util::PI<float> * t - util::PI<float> / 2) + 1) / 2 ;// ease-in/ease-out 계산

    //smin부터 smax까지 ease-in/ease-out 애니메이션
    float tileSize = float(settings.MIN_TILE_SIZE + e * (settings.MAX_TILE_SIZE - settings.MIN_TILE_SIZE));

    if (settings.animate_procedural_texture == true)
    {
        settings.procedural_tile_size = tileSize;
    }

    if (settings.do_blending == true)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
    {
        glDisable(GL_BLEND);
    }

    /*glCheck(glTextureParameteri(duck_image.GetHandle(), GL_TEXTURE_WRAP_S, GL_REPEAT));
    glCheck(glTextureParameteri(duck_image.GetHandle(), GL_TEXTURE_WRAP_T, GL_REPEAT));*/

}

void Tutorial_5::Draw()
{
   // TODO
    glCheck(glClear(GL_COLOR_BUFFER_BIT));
    shader.Use();
    shader.SendUniform("uUseInterpolatedColor",settings.modulate_color);
    shader.SendUniform("procedural", settings.apply_procedural_texture);
    shader.SendUniform("uTileSize", settings.procedural_tile_size);
    shader.SendUniform("Scale", settings.tex_coord_scale);
    duck_image.UseForSlot(0);
    shader.SendUniform("use_texture", settings.use_texture);
    shader.SendUniform("uTex2d", 0);
    fullscreen_quad.Use();
    GLDrawIndexed(fullscreen_quad);
    fullscreen_quad.Use(false);

    shader.Use(false);
}

void Tutorial_5::ImGuiDraw()
{
    ImGui::Begin("Program Info");
    {
        ImGui::LabelText("FPS", "%.1f", double(timing.fps));

        ImGui::Checkbox("Modulate Color", &settings.modulate_color);
        ImGui::SameLine();
        ImGui::Checkbox("Procedural Texture", &settings.apply_procedural_texture);
        ImGui::SameLine();
        ImGui::Checkbox("Duck Texture", &settings.use_texture);
        ImGui::Separator();

        if (settings.apply_procedural_texture)
        {
            ImGui::Checkbox("Animate Procedural Size", &settings.animate_procedural_texture);

            if (ImGui::SliderFloat("Procedural Tile Size", &settings.procedural_tile_size, Settings::MIN_TILE_SIZE, Settings::MAX_TILE_SIZE, "%2.1f"))
            {
                settings.animate_procedural_texture = false;
            }
            ImGui::Separator();
        }
        if (settings.use_texture)
        {
            ImGui::Checkbox("Blending", &settings.do_blending);
            imgui_pick_filtering();
            ImGui::SliderFloat("Tex Coord Scale", &settings.tex_coord_scale, 0.25f, 10.0f, "%2.1f");
            imgui_pick_wrapping("Wrapping for S Direction", GLTexture::S);
            imgui_pick_wrapping("Wrapping for T Direction", GLTexture::T);
            ImGui::Separator();
        }
    }
    ImGui::End();
}

void Tutorial_5::HandleEvent(SDL_Window& sdl_window, const SDL_Event& event)
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

void Tutorial_5::on_window_resized(SDL_Window& sdl_window) noexcept
{
    int width = 0, height = 0;
    SDL_GL_GetDrawableSize(&sdl_window, &width, &height);
    glCheck(glViewport(0, 0, width, height));
}

void Tutorial_5::update_timing() noexcept
{
    const auto current_time = SDL_GetTicks64();
    const auto delta_ms     = current_time - timing.prev_time;
    timing.ElapsedTime += delta_ms / 1'000.0f;

    timing.prev_time      = current_time;
    const auto elapsed_ms = current_time - timing.start_time;

    timing.count++;
    if (elapsed_ms > 1'000)
    {
        timing.fps        = timing.count / (elapsed_ms / 1'000.0f);
        timing.start_time = current_time;
        timing.count      = 0;
    }
}

void Tutorial_5::create_fullscreen_quad()
{
    
    // TODO
    constexpr std::array positions = {
        vec2{-1.0f, -1.0f},  // bottom left
        vec2{1.0f, -1.0f},   // bottom right
        vec2{-1.0f, 1.0f},   // top left
        vec2{1.0f, 1.0f}     // top right
    };

    constexpr std::array texCoords = {
        vec2{0.0f, 0.0f},  // bottom left
        vec2{1.0f, 0.0f},  // bottom right
        vec2{0.0f, 1.0f},  // top left
        vec2{1.0f, 1.0f}   // top right
    };

    constexpr auto positions_byte_size = static_cast<long long>(sizeof(vec2) * positions.size());
    constexpr auto texCoords_byte_size = static_cast<long long>(sizeof(vec2) * texCoords.size());

    constexpr std::array colors = {
        color3{1, 0, 0},  // bottom left - red
        color3{0, 1, 0},  // bottom right - green
        color3{0, 0, 1},  // top left - blue
        color3{1, 1, 1}   // top right - white
    };

    constexpr auto colors_byte_size = static_cast<long long>(sizeof(color3) * colors.size());
    constexpr auto buffer_size = positions_byte_size + colors_byte_size + texCoords_byte_size;

    GLVertexBuffer buffer(buffer_size);
    buffer.SetData(std::span(positions));
    buffer.SetData(std::span(texCoords), positions_byte_size);
    buffer.SetData(std::span(colors), positions_byte_size + texCoords_byte_size);

    GLAttributeLayout position;
    position.component_type = GLAttributeLayout::Float;
    position.component_dimension = GLAttributeLayout::_2;
    position.normalized = false;
    position.vertex_layout_location = 0;
    position.stride = sizeof(vec2);
    position.offset = 0;
    position.relative_offset = 0;

    GLAttributeLayout texCoord;
    texCoord.component_type = GLAttributeLayout::Float;
    texCoord.component_dimension = GLAttributeLayout::_2;
    texCoord.normalized = false;
    texCoord.vertex_layout_location = 2;
    texCoord.stride = sizeof(vec2);
    texCoord.offset = positions_byte_size;
    texCoord.relative_offset = 0;

    GLAttributeLayout color;
    color.component_type = GLAttributeLayout::Float;
    color.component_dimension = GLAttributeLayout::_3;
    color.normalized = false;
    color.vertex_layout_location = 1;
    color.stride = sizeof(color3);
    color.offset = positions_byte_size + texCoords_byte_size;
    color.relative_offset = 0;

    fullscreen_quad.AddVertexBuffer(std::move(buffer), { position, texCoord, color });
    fullscreen_quad.SetPrimitivePattern(GLPrimitive::TriangleStrip);

    constexpr std::array<unsigned int, 4> indices = {0, 1, 2, 3};
    GLIndexBuffer                     index_buffer(indices);
    fullscreen_quad.SetIndexBuffer(std::move(index_buffer));

}

void Tutorial_5::imgui_pick_filtering()
{
    constexpr const char*          items[]             = {"NearestPixel", "Linear"};
    constexpr GLTexture::Filtering types[]             = {GLTexture::NearestPixel, GLTexture::Linear};
    const char* const              combo_preview_value = items[settings.filtering_index];
    if (ImGui::BeginCombo("Texture Filtering", combo_preview_value, 0))
    {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++)
        {
            const bool is_selected = (settings.filtering_index == n);
            if (ImGui::Selectable(items[n], is_selected))
            {
                settings.filtering_index = n;
                duck_image.SetFiltering(types[n]);
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}

void Tutorial_5::imgui_pick_wrapping(const char* label, GLTexture::Coordinate which_coord)
{
    constexpr const char*         items[] = {"Repeat", "ClampToEdge", "ClampToBorder", "MirroredRepeat", "MirrorClampToEdge"};
    constexpr GLTexture::Wrapping types[] = {GLTexture::Repeat, GLTexture::ClampToEdge, GLTexture::ClampToBorder, GLTexture::MirroredRepeat, GLTexture::MirrorClampToEdge};
    const char* const             combo_preview_value = items[settings.wrap_index[which_coord]];

    if (ImGui::BeginCombo(label, combo_preview_value, 0))
    {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++)
        {
            const bool is_selected = (settings.wrap_index[which_coord] == n);
            if (ImGui::Selectable(items[n], is_selected))
            {
                settings.wrap_index[which_coord] = n;
                duck_image.SetWrapping(types[n], which_coord);
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}//