/*
 * Rudy Castan / Jimin Lim
 * CS200
 * Fall 2022
 */

#include "Tutorial_2.h"

#include "Random.h"
#include "angles.h"
#include "color3.h"
#include "glCheck.h"
#include "vec2.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <cmath>
#include <imgui.h>
#include <iostream>


util::owner<IProgram*> create_program(int viewport_width, int viewport_height)
{
    return new Tutorial_2(viewport_width, viewport_height);
}

Tutorial_2::Tutorial_2(int viewport_width, int viewport_height)
{
    // TODO
    set_viewports(viewport_width, viewport_height);

    timing.start_time = SDL_GetTicks64();
    timing.prev_time  = timing.start_time;

    // Part 3: create shader for our models
    shader = GLShader("Basic Shader", { {GLShader::VERTEX, "assets/shaders/pass_thru_pos2d_clr.vert"}, {GLShader::FRAGMENT, "assets/shaders/basic_vtx_clr_attribute.frag"} });

    // Part 4: build our models
    create_points_model();
    create_lines_model();
    create_trifan_model();
    create_tristrip_model();
}

void Tutorial_2::Update()
{
    update_timing();
}


void Tutorial_2::Draw()
{
    // Part 1: clear color buffer using OpenGL function glClear
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glCheck(glClear(GL_COLOR_BUFFER_BIT));

    // Use Shader
    shader.Use();

    // Part 2: Render distinct geometry to four viewports:
    // Use OpenGL function glViewport to specify appropriate top left portion of display screen
    glCheck(glViewport(viewports[TopLeft].x, viewports[TopLeft].y, viewports[TopLeft].width, viewports[TopLeft].height));
    {
        glCheck(glPointSize(10.0f));
        models[TopLeft].Use();
        constexpr int layout_location = 1;
        glCheck(glVertexAttrib3f(layout_location, 1.f, 0.0f, 0.f)); // red for color attribute for points
        GLDrawVertices(models[TopLeft]);
        glCheck(glPointSize(1.5f)); //점크기 변경
    }
    // TODO: Later draw other three parts

    glCheck(glViewport(viewports[TopRight].x, viewports[TopRight].y, viewports[TopRight].width, viewports[TopRight].height));
    {
        glCheck(glLineWidth(1.5f));
        models[TopRight].Use();
        constexpr int layout_location = 1;
        glCheck(glVertexAttrib3f(layout_location, 0.f, 0.0f, 1.f)); // blue for color attribute for lines        
        GLDrawVertices(models[TopRight]);
        glCheck(glLineWidth(1.0f));
    }

    glCheck(glViewport(viewports[BottomLeft].x, viewports[BottomLeft].y, viewports[BottomLeft].width, viewports[BottomLeft].height));
    {
        glCheck(glLineWidth(3.0f));
        models[BottomLeft].Use();
        constexpr int layout_location = 1;
        glCheck(glVertexAttrib3f(layout_location, 0.f, 0.0f, 1.f)); // blue for color attribute for lines        
        GLDrawVertices(models[BottomLeft]);
        glCheck(glLineWidth(1.0f));
    }

    glCheck(glViewport(viewports[BottomRight].x, viewports[BottomRight].y, viewports[BottomRight].width, viewports[BottomRight].height));
    {
        glCheck(glLineWidth(3.0f));
        models[BottomRight].Use();
        constexpr int layout_location = 1;
        glCheck(glVertexAttrib3f(layout_location, 0.f, 0.0f, 1.f)); // blue for color attribute for lines        
        glDrawElements(GL_TRIANGLE_STRIP, models[BottomRight].GetIndicesCount(), GL_UNSIGNED_SHORT, 0);
        glCheck(glLineWidth(1.0f));
    }

    shader.Use(false);
}

void Tutorial_2::ImGuiDraw()
{
    ImGui::Begin("Program Info");
    {
        ImGui::LabelText("FPS", "%.1f", timing.fps);
        ImGui::LabelText("GL_POINTS", "# Points    %4d", models[TopLeft].GetVertexCount());
        ImGui::LabelText("GL_LINES", "# Lines     %4d # Verts   %4d", models[1].GetVertexCount() / 2, models[1].GetVertexCount());
        ImGui::LabelText("GL_TRIANGLE_FAN", "# Triangles %4d # Verts %4d", models[BottomLeft].GetVertexCount() - 2, models[BottomLeft].GetVertexCount());
        ImGui::LabelText("GL_TRIANGLE_STRIP", "# Triangles %4d # Indices %4d", models[3].GetIndicesCount() - 2, models[3].GetIndicesCount());
    }
    ImGui::End();
}

void Tutorial_2::HandleEvent(SDL_Window& sdl_window, const SDL_Event& event)
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

void Tutorial_2::on_window_resized(SDL_Window& sdl_window) noexcept
{
    int width = 0, height = 0;
    SDL_GL_GetDrawableSize(&sdl_window, &width, &height);
    set_viewports(width, height);
}

void Tutorial_2::update_timing() noexcept
{
    const auto current_time = SDL_GetTicks64();

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

void Tutorial_2::set_viewports(int width, int height)
{
    // Calculate coordinates and dimensions for each viewport
    const int viewportWidth = width / 2;
    const int viewportHeight = height / 2;

    viewports[TopLeft] = { 0, height - viewportHeight, viewportWidth, viewportHeight };
    viewports[TopRight] = { viewportWidth, height - viewportHeight, viewportWidth, viewportHeight };
    viewports[BottomLeft] = { 0, 0, viewportWidth, viewportHeight };
    viewports[BottomRight] = { viewportWidth, 0, viewportWidth, viewportHeight };
}

void Tutorial_2::create_points_model()
{
    /*
    constexpr int slices = 20;

    std::vector<vec2> pos_vtx(slices + 1);

    float interval_length = 2.0f / static_cast<float>(slices);

    for (int i = 0; i <= slices; ++i)
    {
        float x = -1.0f + interval_length * static_cast<float>(i);
        float y = 0.0f;
        pos_vtx[i] = vec2(x, y);
    }*/

    constexpr int stacks = 20;
    constexpr int slices = 20;
    constexpr int count = (stacks + 1) * (slices + 1);

    std::vector<vec2> pos_vtx(count);//점 저장

    float stack_interval = 2.0f / static_cast<float>(stacks);
    float slice_interval = 2.0f / static_cast<float>(slices);

    int pointIndex = 0;
    for (int i = 0; i <= stacks; ++i)
    {
        float y = -1.0f + stack_interval * static_cast<float>(i);

        for (int j = 0; j <= slices; ++j)
        {
          
            float x = -1.0f + slice_interval * static_cast<float>(j);
            pos_vtx[pointIndex++] = vec2(x, y);
        }
    }

    
    models[TopLeft].SetVertexCount(static_cast<int>(pos_vtx.size()));

    GLAttributeLayout position;//모델의 위치를 설정하는 코드
    position.component_dimension = GLAttributeLayout::_2; //버택스 구성 요소(x, y, z)의 수  --> 2D 좌표를 표현
    position.component_type = GLAttributeLayout::Float; //좌표 구성요소의 데이터 타입
    position.normalized = false;
    position.offset = 0;
    position.relative_offset = 0;
    position.stride = sizeof(vec2);
    position.vertex_layout_location = 0;

    models[TopLeft].AddVertexBuffer(GLVertexBuffer(std::span{ pos_vtx }), { position }); //버텍스 데이터를 GPU로 전송하는 데 사용되는 버퍼

    models[TopLeft].SetPrimitivePattern(GLPrimitive::Points); ///렌더링할 기본 도형(예: 삼각형, 선분)의 패턴이나 스타일을 설정
}


void Tutorial_2::create_lines_model()
{
    /*constexpr int slices = 40;
    constexpr int count = (slices + 1) * 2;

    std::vector<vec2> pos_vtx(count);

    unsigned index = 0;

    constexpr float interval_length = 2.0f / static_cast<float>(slices);

    for (int col = 0; col <= slices; ++col)
    {
        float x = -1.0f + interval_length * static_cast<float>(col);

        pos_vtx[index++] = vec2(x, 1.0f);  
        pos_vtx[index++] = vec2(x, -1.0f); 
    }*/

    constexpr int slices = 40;
    constexpr int stacks = 40;

    constexpr int verticalLineCount = (stacks + 1) * 2;
    constexpr int horizontalLineCount = (slices + 1) * 2;

    constexpr int count = verticalLineCount + horizontalLineCount;

    std::vector<vec2> pos_vtx(count);

    unsigned index = 0;
    constexpr float sliceInterval = 2.0f / static_cast<float>(slices);
    constexpr float stackInterval = 2.0f / static_cast<float>(stacks);

    for (int col = 0; col <= slices; ++col)
    {
        float x = -1.0f + sliceInterval * static_cast<float>(col);
        pos_vtx[index++] = vec2(x, -1.0f); // Start
        pos_vtx[index++] = vec2(x, 1.0f);  // End
    }

    for (int row = 0; row <= stacks; ++row)
    {
        float y = -1.0f + stackInterval * static_cast<float>(row);
        pos_vtx[index++] = vec2(-1.0f, y); // Start 
        pos_vtx[index++] = vec2(1.0f, y);  // End 
    }

    models[TopRight].SetVertexCount(static_cast<int>(pos_vtx.size()));

    GLAttributeLayout position;
    position.component_dimension = GLAttributeLayout::_2;
    position.component_type = GLAttributeLayout::Float;
    position.normalized = false;
    position.offset = 0;
    position.relative_offset = 0;
    position.stride = sizeof(vec2);
    position.vertex_layout_location = 0;

    models[TopRight].AddVertexBuffer(GLVertexBuffer(std::span{ pos_vtx }), { position });

    models[TopRight].SetPrimitivePattern(GLPrimitive::Lines);

}


void Tutorial_2::create_trifan_model()
{
    constexpr int slices = 30; //원을 얼마나 나눌지

    //정점 수 계산 (slices + 2)
    constexpr int vertexCount = slices + 2;

    // 정점 위치와 색상을 저장할 벡터 생성
    std::vector<vec2> pos_vtx(vertexCount);
    std::vector<color3> color_vtx(vertexCount);

    // Step 2: 정점의 위치와 색상 계산
    for (int i = 1; i <= slices; ++i)
    {
        for (int j = 0; j <= slices; ++j)
        {
            // 무작위 색상 생성
            color_vtx[j] = color3{ util::random(), util::random(), util::random() };

            //0부터 2파이까지 균일하게 분포
            float theta = static_cast<float>(i) / static_cast<float>(slices) * 2.0f * 3.14159265359f;//slices 수로 나누어 각 점의 위치 계산

            // x, y 좌표 계산
            float x = std::cos(theta);
            float y = std::sin(theta);

            // 정점 위치 설정
            pos_vtx[i] = vec2(x, y);
        }
        
    }

    std::vector<unsigned short> idx;
    idx.push_back(0); // Pivot vertex
    for (unsigned short i = 1; i <= slices; ++i) 
    {
        idx.push_back(i);
    }
    idx.push_back(1);


    // 마지막 정점과 두 번째 정점이 동일
    pos_vtx[vertexCount - 1] = pos_vtx[1];
    color_vtx[vertexCount - 1] = color_vtx[1];

    //Vertex Buffer 생성
    GLVertexBuffer posBuffer(std::span{ pos_vtx });
    GLVertexBuffer colorBuffer(std::span{ color_vtx });

    models[BottomLeft].SetVertexCount(vertexCount);

    // 위치
    GLAttributeLayout positionLayout;
    positionLayout.component_dimension = GLAttributeLayout::_2;
    positionLayout.component_type = GLAttributeLayout::Float;
    positionLayout.normalized = false;
    positionLayout.offset = 0;
    positionLayout.relative_offset = 0;
    positionLayout.stride = sizeof(vec2);
    positionLayout.vertex_layout_location = 0;

    //색상
    GLAttributeLayout colorLayout;
    colorLayout.component_dimension = GLAttributeLayout::_3;
    colorLayout.component_type = GLAttributeLayout::Float;
    colorLayout.normalized = false;
    colorLayout.offset = 0;
    colorLayout.relative_offset = 0;
    colorLayout.stride = sizeof(color3);
    colorLayout.vertex_layout_location = 1;

    models[BottomLeft].AddVertexBuffer(std::span{ pos_vtx }, { positionLayout });
    models[BottomLeft].AddVertexBuffer(std::span{ color_vtx }, { colorLayout });

    models[BottomLeft].SetPrimitivePattern(GLPrimitive::TriangleFan);
}


void Tutorial_2::create_tristrip_model()
{
    // Constants for grid size
    constexpr short stacks = 15; //세로줄 개수
    constexpr short slices = 10; //가로줄 개수

    // Calculate the total number of vertices
    constexpr unsigned count = (stacks + 1) * (slices + 1); // 격자 내의 모든 정점 수

    // Create vertex arrays for positions and colors
    std::vector<vec2> pos_vtx(count); //정점들의 위치 정보를 저장 -> 전체 정점 수 만큼
    std::vector<color3> clr_vtx(count); //정점들의 색상 정보를 저장하는 배열

    // Calculate grid spacing in NDC
    constexpr float v = 2.0f / static_cast<float>(stacks); //세로줄 간격
    constexpr float u = 2.0f / static_cast<float>(slices); //가로줄 간격

    // 모든 격자 셀에 대한 정점 위치와 색상계산 저장
    for (unsigned row = 0, index = 0; row <= stacks; ++row) //가로 개수 순회
    {
        for (unsigned column = 0; column <= slices; ++column) //세로 개수 순회
        {
            pos_vtx[index] = vec2{ u * static_cast<float>(column) - 1.f, v * static_cast<float>(row) - 1.f };
            clr_vtx[index++] = color3{ util::random(), util::random(), util::random() };
        }
    }

    std::vector<unsigned short> idx_buffer; //삼각형 스트립 인덱스 버퍼 -->> 정점의 순서 연결 결정

    for (unsigned stack = 0; stack < stacks; stack++) // 가로 개수 순회
    {
        for (unsigned slice = 0; slice <= slices; slice++)
        {
            idx_buffer.push_back(static_cast<unsigned short>((stack + 1) * (slices + 1) + slice));
            idx_buffer.push_back(static_cast<unsigned short>(stack * (slices + 1) + slice));
        }

        if (stack < stacks - 1) {
            idx_buffer.push_back(static_cast<unsigned short>(slices + (slices + 1) * stack));
            idx_buffer.push_back(static_cast<unsigned short>((slices + 1) * (stack + 2)));
        }
    }

    GLVertexBuffer posBuffer(std::span{ pos_vtx });
    GLVertexBuffer colorBuffer(std::span{ clr_vtx });
    GLIndexBuffer indexBuffer(std::span{ idx_buffer });

    GLAttributeLayout positionLayout;
    positionLayout.component_dimension = GLAttributeLayout::_2;
    positionLayout.component_type = GLAttributeLayout::Float;
    positionLayout.normalized = false;
    positionLayout.offset = 0;
    positionLayout.relative_offset = 0;
    positionLayout.stride = sizeof(vec2);
    positionLayout.vertex_layout_location = 0;

    GLAttributeLayout colorLayout;
    colorLayout.component_dimension = GLAttributeLayout::_3;
    colorLayout.component_type = GLAttributeLayout::Float;
    colorLayout.normalized = false;
    colorLayout.offset = 0;
    colorLayout.relative_offset = 0;
    colorLayout.stride = sizeof(color3);
    colorLayout.vertex_layout_location = 1;

    models[BottomRight].AddVertexBuffer(std::span{ pos_vtx }, { positionLayout });
    models[BottomRight].AddVertexBuffer(std::span{ clr_vtx }, { colorLayout });
    models[BottomRight].SetIndexBuffer(std::move(indexBuffer));
    models[BottomRight].SetPrimitivePattern(GLPrimitive::TriangleStrip);

}

