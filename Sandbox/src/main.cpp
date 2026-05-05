#include <print>

#include "Mocca/core/application.h"
#include "Mocca/core/input.h"

#include "Sandbox/experiments/test_feature.h"


// TODO: Wrap everything in namespaces

class MyApp : public Application
{
public:
    MyApp(uint32_t width, uint32_t height, const std::string& title) : Application(width, height, title) {}

    void onInit() override
    {
        m_renderer.pushFeature(std::make_unique<TestFeature>());
    }

    void onUpdate(float deltaTime) override
    {
        static bool wasPressed = false;
        bool isPressed = Input::isKeyDown(Key::Space);

        if(isPressed && !wasPressed)
        {
            auto* triangle = m_renderer.getFeature<TestFeature>();
            if(triangle)
            {
                bool currentState = triangle->isEnabled();
                triangle->setEnabled(!currentState);
                std::println("Triangle is now: {}", !currentState ? "OFF" : "ON");
            }
        }
        wasPressed = isPressed;
    }

private:
};


int main(int argc, char* argv[])
{

    MyApp app(1280, 720, "Vulkan App");

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