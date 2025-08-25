/*
 * Rudy Castan , Jimin Lim
 * CS200
 * Fall 2022
 */

#include "Tutorial_3.h"

#include "Random.h"
#include "angles.h"
#include "color3.h"
#include "glCheck.h"
#include "vec2.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <algorithm>
#include <imgui.h>
#include <iostream>
#include <random>

util::owner<IProgram*> create_program(int viewport_width, int viewport_height)
{
    return new Tutorial_3(viewport_width, viewport_height);
}

Tutorial_3::Tutorial_3(int viewport_width, int viewport_height)
{
    glCheck(glViewport(0, 0, viewport_width, viewport_height));
    glCheck(glClearColor(0.392f, 0.584f, 0.929f, 1));

    create_models();

    shader = GLShader("Basic Shader", { {GLShader::VERTEX, "assets/shaders/model_to_ndc_pos2d_clr.vert"}, {GLShader::FRAGMENT, "assets/shaders/basic_vtx_clr_attribute.frag"} });
    objects.reserve(MAX_NUMBER_OBJECTS);

    timing.start_time = SDL_GetTicks64();
    timing.prev_time = timing.start_time;

   Object go;
 
   // objects.push_back(Object{ 0, 0, vec2{ 0.3f } ,  vec2{ 10.0f }, static_cast<ModelType>(static_cast<int>(util::random(1000)) % 2) });

    int Which_modleswl = (static_cast<int>(util::random(1000)) % 2);

    if (Which_modleswl > 0)
    {
        go.which_model = Rectangle;
        BoxNum++;
    }
    else
    {
        go.which_model = MysteryShape;
        MysteryNum++;
    }

    std::default_random_engine dre; 
    std::uniform_real_distribution<float> urdf(-1.0, 1.0);

    go.position.x = util::random(WORLD_SIZE_MIN, WORLD_SIZE_MAX);
    go.position.y = util::random(WORLD_SIZE_MIN, WORLD_SIZE_MAX);
    go.scale.x = util::random(OBJ_SIZE_MIN, OBJ_SIZE_MAX);
    go.scale.y = util::random(OBJ_SIZE_MIN, OBJ_SIZE_MAX);
    go.rotation = urdf(dre) * util::TWO_PI<float>;
    go.rotation_speed = util::random(ROT_SPEED_MIN, ROT_SPEED_MAX) * util::to_radians(30.0f);

    objects.push_back(go);
}

void Tutorial_3::Update()
{
    const auto current_time = SDL_GetTicks64();

    const auto elapsed_ms = current_time - timing.prev_time;
    timing.prev_time = current_time;

    float deltaTime = elapsed_ms / 1000.0f; // 초 단위로 deltaTime 계산

    for (Object& obj : objects)
    {
        obj.rotation += obj.rotation_speed * deltaTime;
    }
}


namespace
{
    std::span<const float, 3 * 3> to_span(const mat3& m)
    {
        return std::span<const float, 3 * 3>(&m.elements[0][0], 9);
    }
}


void Tutorial_3::Draw()
{
    // TODO
    glCheck(glClear(GL_COLOR_BUFFER_BIT));

    if (poly_mode.type == PolyMode::Point)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        glPointSize(5.0f);
    }
    else if (poly_mode.type == PolyMode::Line)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(2.0f);
    }
    else if (poly_mode.type == PolyMode::Fill)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    shader.Use();

    for (const auto& object : objects)
    {
        models[object.which_model].Use();

        mat3 translationMatrix = mat3::build_translation(object.position);
        mat3 rotationMatrix = mat3::build_rotation(object.rotation);
        mat3 scaleMatix = mat3::build_scale(object.scale);

        mat3 modelToWorld = translationMatrix * rotationMatrix * scaleMatix;

        const float SCREEN_WIDTH = 2.0F * WORLD_SIZE_MAX;
        const float SCREEN_HIGHT = 2.0F * WORLD_SIZE_MAX;

        mat3 scaleToWorld = mat3::build_scale(2.0f / SCREEN_WIDTH, 2.0f / SCREEN_HIGHT);
        mat3 translateToWorld = mat3::build_translation(-1.0f, -1.0f);
        mat3 WORLD_TO_NDC = scaleToWorld;
        mat3 modelToNDC = WORLD_TO_NDC * modelToWorld;

        shader.SendUniform("uModelToNDC", to_span(modelToNDC));

        //GLDrawVertices(models[object.which_model]);

        glDrawElements(GL_TRIANGLES, models[object.which_model].GetIndicesCount(), GL_UNSIGNED_SHORT, 0);

        models[object.which_model].Use(false);
    }

    shader.Use(false);

}

void Tutorial_3::ImGuiDraw()
{
    ImGui::Begin("Program Info");
    {
        ImGui::LabelText("FPS", "%.1f", timing.fps);

        if (objects.size() == MAX_NUMBER_OBJECTS)
        {
            ImGui::BeginDisabled();
            ImGui::ArrowButton("+", ImGuiDir_Up);
            ImGui::EndDisabled();
        }
        else
        {
            if (const bool increase = ImGui::ArrowButton("+", ImGuiDir_Up); increase)
            {
                increase_number_of_objects();
            }
        }
        ImGui::SameLine();
        if (objects.size() == 1)
        {
            ImGui::BeginDisabled();
            ImGui::ArrowButton("-", ImGuiDir_Down);
            ImGui::EndDisabled();
        }
        else
        {
            if (const bool decrease = ImGui::ArrowButton("-", ImGuiDir_Down); decrease)
            {
                decrease_number_of_objects();
            }
        }
        ImGui::SameLine();
        ImGui::Text("%s %lu", "Size", static_cast<unsigned long>(objects.size()));
        ImGui::LabelText("# Boxes", "%d", BoxNum);
        ImGui::LabelText("# Mystery Shapes", "%d", MysteryNum);


        constexpr const char* items[] = { "GL_POINT", "GL_LINE", "GL_FILL" };
        constexpr PolyMode    types[] = { PolyMode::Point, PolyMode::Line, PolyMode::Fill };
        const char* const     combo_preview_value = items[poly_mode.item_current_idx];
        if (ImGui::BeginCombo("PolygonMode", combo_preview_value, 0))
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                const bool is_selected = (poly_mode.item_current_idx == n);
                if (ImGui::Selectable(items[n], is_selected))
                {
                    poly_mode.item_current_idx = n;
                    poly_mode.type = types[n];
                }
                if (is_selected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }
    ImGui::End();
}

void Tutorial_3::HandleEvent(SDL_Window& sdl_window, const SDL_Event& event)
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

void Tutorial_3::on_window_resized(SDL_Window& sdl_window) noexcept
{
    int width = 0, height = 0;
    SDL_GL_GetDrawableSize(&sdl_window, &width, &height);
    glCheck(glViewport(0, 0, width, height));
}

void Tutorial_3::update_timing() noexcept
{
    const auto current_time = SDL_GetTicks64();

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



void Tutorial_3::create_models()
{
    { // TODO: rectangle
        std::array<vec2, 4> pos_vtx = { {
            { -0.5f, -0.5f }, // Bottom-left
            {  0.5f, -0.5f }, // Bottom-right
            {  0.5f,  0.5f }, // Top-right
            { -0.5f,  0.5f }  // Top-left
        } };

        std::array<vec3, 4> color_vtx = { {
            { 1.0f, 0.0f, 0.0f }, // Red for Bottom-left
            { 0.0f, 1.0f, 0.0f }, // Green for Bottom-right
            { 0.0f, 0.0f, 1.0f }, // Blue for Top-right
            { 1.0f, 1.0f, 0.0f }  // Yellow for Top-left
        } };

        std::array<unsigned short, 6> indices = { 0, 1, 2, 2, 3, 0 };

        models[ModelType::Rectangle].SetVertexCount(static_cast<int>(pos_vtx.size()));

        GLVertexBuffer position_buffer(std::span{pos_vtx});
        GLVertexBuffer color_buffer(std::span{color_vtx});
        GLIndexBuffer index_buffer(std::span{indices}); // 인덱스 버퍼

        // Position Attribute
        GLAttributeLayout position;
        position.component_dimension = GLAttributeLayout::_2;
        position.component_type = GLAttributeLayout::Float;
        position.normalized = false;
        position.offset = 0;
        position.relative_offset = 0;
        position.stride = sizeof(vec2);
        position.vertex_layout_location = 0;
        models[ModelType::Rectangle].AddVertexBuffer(std::move(position_buffer), { position });

        // Color Attribute
        GLAttributeLayout color;
        color.component_dimension = GLAttributeLayout::_3;
        color.component_type = GLAttributeLayout::Float;
        color.normalized = false;
        color.offset = 0;
        color.relative_offset = 0;
        color.stride = sizeof(vec3);
        color.vertex_layout_location = 1;
        models[ModelType::Rectangle].AddVertexBuffer(std::move(color_buffer), { color });

        models[ModelType::Rectangle].SetIndexBuffer(std::move(index_buffer));

        models[ModelType::Rectangle].SetPrimitivePattern(GLPrimitive::TriangleStrip);

        GLuint vaoid;
        glCheck(glCreateVertexArrays(1, &vaoid));
        glCheck(glBindVertexArray(vaoid)); // VAO를 활성화
        glCheck(glEnableVertexArrayAttrib(vaoid, 0));
        glCheck(glVertexArrayVertexBuffer(vaoid, 0, position_buffer.GetHandle(), 0, sizeof(vec2)));
        glCheck(glVertexArrayAttribFormat(vaoid, 0, 2, GL_FLOAT, GL_FALSE, 0));
        glCheck(glVertexArrayAttribBinding(vaoid, 0, 0));
        glCheck(glEnableVertexArrayAttrib(vaoid, 1));
        glCheck(glVertexArrayVertexBuffer(vaoid, 1, color_buffer.GetHandle(), 0, sizeof(color3)));
        glCheck(glVertexArrayAttribFormat(vaoid, 1, 3, GL_FLOAT, GL_FALSE, 0));
        glCheck(glVertexArrayAttribBinding(vaoid, 1, 1));

        // VAO 설정이 완료되었으므로 다시 VAO를 바인딩 해제
        glCheck(glBindVertexArray(0));
    }

    {
        std::array<vec2, 9> pos_vtx = { {
         { 0.0f, 0.0f },  // 0: Center of the shape
         { 0.0f, 0.7f },  // 1
         { -0.2f, 0.5f }, // 2
         { 0.2f, 0.5f },  // 3
         { -0.5f, 0.0f }, // 4
         { 0.5f, 0.0f },  // 5
         { -0.2f, -0.5f },// 6
         { 0.2f, -0.5f }, // 7
         { 0.0f, -0.7f }  // 8
     } };

        std::array<vec3, 9> color_vtx = { {
        { 1.0f, 1.0f, 0.0f }, //center
        { 1.0f, 0.0f, 0.0f }, // Red
        { 0.0f, 1.0f, 0.0f }, // Green
        { 0.0f, 0.0f, 1.0f }, // Blue
        { 1.0f, 1.0f, 1.0f }, // White
        { 1.0f, 0.0f, 1.0f }, // Magenta
        { 0.0f, 1.0f, 1.0f }, // Cyan
        { 0.5f, 0.5f, 0.0f }, // Olive
        { 0.5f, 0.0f, 0.5f }  // Purple
    } };


        std::array<unsigned short, 21> indices = {
            0, 1, 2,
            0, 3, 4,
            0, 5, 6,
            0, 7, 8,
            0, 9, 10,
            0, 11, 12,
            0, 13, 14
        };

        models[ModelType::MysteryShape].SetVertexCount(static_cast<int>(pos_vtx.size()));

        GLVertexBuffer position_buffer(std::span{pos_vtx});
        GLVertexBuffer color_buffer(std::span{color_vtx});
        GLIndexBuffer index_buffer(std::span{indices});

        GLAttributeLayout position;
        position.component_dimension = GLAttributeLayout::_2;
        position.component_type = GLAttributeLayout::Float;
        position.normalized = false;
        position.offset = 0;
        position.relative_offset = 0;
        position.stride = sizeof(vec2);
        position.vertex_layout_location = 0;
        models[ModelType::MysteryShape].AddVertexBuffer(std::move(position_buffer), { position });

        GLAttributeLayout color;
        color.component_dimension = GLAttributeLayout::_3;
        color.component_type = GLAttributeLayout::Float;
        color.normalized = false;
        color.offset = 0;
        color.relative_offset = 0;
        color.stride = sizeof(vec3);
        color.vertex_layout_location = 1;
        models[ModelType::MysteryShape].AddVertexBuffer(std::move(color_buffer), { color });

        models[ModelType::MysteryShape].SetIndexBuffer(std::move(index_buffer));

        models[ModelType::MysteryShape].SetPrimitivePattern(GLPrimitive::Triangles);

        GLuint vaoid;
        glCheck(glCreateVertexArrays(1, &vaoid));
        glCheck(glBindVertexArray(vaoid)); // VAO를 활성화
        glCheck(glEnableVertexArrayAttrib(vaoid, 0));
        glCheck(glVertexArrayVertexBuffer(vaoid, 0, position_buffer.GetHandle(), 0, sizeof(vec2)));
        glCheck(glVertexArrayAttribFormat(vaoid, 0, 2, GL_FLOAT, GL_FALSE, 0));
        glCheck(glVertexArrayAttribBinding(vaoid, 0, 0));
        glCheck(glEnableVertexArrayAttrib(vaoid, 1));
        glCheck(glVertexArrayVertexBuffer(vaoid, 1, color_buffer.GetHandle(), 0, sizeof(color3)));
        glCheck(glVertexArrayAttribFormat(vaoid, 1, 3, GL_FLOAT, GL_FALSE, 0));
        glCheck(glVertexArrayAttribBinding(vaoid, 1, 1));

        // VAO 설정이 완료되었으므로 다시 VAO를 바인딩 해제
        glCheck(glBindVertexArray(0));

    }
    
    
}

void Tutorial_3::increase_number_of_objects()
{
    std::default_random_engine dre; 
    std::uniform_real_distribution<float> urdf(-1.0, 1.0);

    size_t preSize = objects.size();
    size_t newSize = (preSize == 0) ? 1 : 2 * preSize;

    newSize = std::min(newSize, static_cast<size_t>(MAX_NUMBER_OBJECTS));

    while (objects.size() < newSize)
    {
        Object go;

        int Which_modleswl = (static_cast<int>(util::random(1000)) % 2);
        if (Which_modleswl > 0)
        {
            go.which_model = Rectangle;
            BoxNum++;
        }
        else
        {
            go.which_model = MysteryShape;
            MysteryNum++;
        }

        go.position.x = util::random(WORLD_SIZE_MIN, WORLD_SIZE_MAX);
        go.position.y = util::random(WORLD_SIZE_MIN, WORLD_SIZE_MAX);
        go.scale.x = util::random(OBJ_SIZE_MIN, OBJ_SIZE_MAX);
        go.scale.y = util::random(OBJ_SIZE_MIN, OBJ_SIZE_MAX);
        go.rotation = urdf(dre) * util::TWO_PI<float>;
        go.rotation_speed = util::random(ROT_SPEED_MIN, ROT_SPEED_MAX) * util::to_radians(30.0f); // 0 ~ 30까지 랜덤으로 가능

        objects.push_back(go);
    }
}

void Tutorial_3::decrease_number_of_objects()
{
    size_t preSize = objects.size();
    size_t newSize = preSize / 2;

    while (objects.size() > newSize)
    {
        Object& firstObject = objects.front();

        if (firstObject.which_model == Rectangle)
        {
            BoxNum--;
        }
        else if (firstObject.which_model == MysteryShape)
        {
            MysteryNum--;
        }

        objects.erase(objects.begin());
    }
}
