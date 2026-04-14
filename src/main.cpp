#include <SDL.h>
#include <iostream>

#include "engine/core/application.h"

int main(int argc, char* argv[])
{

    Application app(1280, 720, "Vulkan App");

    try
    {
        app.run();
    }
    catch (const std::exception& e)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}