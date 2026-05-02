#include <print>

#include "Mocca/core/application.h"


// TODO: Wrap everything in namespaces

int main(int argc, char* argv[])
{

    Application app(1280, 720, "Vulkan App");

    try
    {
        app.run();
    }
    catch(const std::exception& e)
    {
        std::println(stderr, "Fatal error: {}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}