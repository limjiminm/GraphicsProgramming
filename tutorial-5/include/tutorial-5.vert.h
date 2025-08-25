//#pragma once
//
//const char* vertexShader = R"glsl(
//#version 330 core
//
//layout(location = 0) in vec3 inPosition;  // 꼭짓점 위치
//layout(location = 1) in vec4 inColor;     // 꼭짓점 색상
//layout(location = 2) in vec2 inTexCoord;  // 텍스처 좌표
//
//out vec4 fragColor;       // 프래그먼트 쉐이더로 전달할 보간된 색상
//out vec2 texCoord;        // 프래그먼트 쉐이더로 전달할 텍스처 좌표
//
//void main()
//{
//    gl_Position = vec4(inPosition, 1.0);  // 꼭짓점 위치 설정
//    fragColor = inColor;                  // 꼭짓점 색상 설정
//    texCoord = inTexCoord;                // 텍스처 좌표 설정
//}
//)glsl";
