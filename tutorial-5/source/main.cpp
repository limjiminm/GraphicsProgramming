#include "GLApp.h"

#include <iostream>

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
try
{
    // TODO: update name
    GLApp OpenGLApplication("Tutorial 5 for Jimin Lim: Texture Mapping");

    while (!OpenGLApplication.IsDone())
    {
        OpenGLApplication.Update();
    }

    return 0;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << '\n';
    return -1;
}
