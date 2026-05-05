#pragma once

#include "Mocca/core/application.h"
#include "Mocca/core/input.h"

#include "Sandbox/experiments/test_feature.h"

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
