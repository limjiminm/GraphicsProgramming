//#pragma once
//
//const char* vertexShader = R"glsl(
//#version 330 core
//
//layout(location = 0) in vec3 inPosition;  // ������ ��ġ
//layout(location = 1) in vec4 inColor;     // ������ ����
//layout(location = 2) in vec2 inTexCoord;  // �ؽ�ó ��ǥ
//
//out vec4 fragColor;       // �����׸�Ʈ ���̴��� ������ ������ ����
//out vec2 texCoord;        // �����׸�Ʈ ���̴��� ������ �ؽ�ó ��ǥ
//
//void main()
//{
//    gl_Position = vec4(inPosition, 1.0);  // ������ ��ġ ����
//    fragColor = inColor;                  // ������ ���� ����
//    texCoord = inTexCoord;                // �ؽ�ó ��ǥ ����
//}
//)glsl";
