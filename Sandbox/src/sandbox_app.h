#pragma once

#include "core/input.h"
#include "engine/application.h"

#include "experiments/test_feature.h"

#include <print>

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
