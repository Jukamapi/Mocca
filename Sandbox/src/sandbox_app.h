#pragma once

#include "core/input.h"
#include "engine/application.h"
#include "experiments/imgui_feature.h"
#include "experiments/test_feature.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

#include <print>

class MyApp : public Application
{
public:
    MyApp(uint32_t width, uint32_t height, const std::string& title)
        : Application(width, height, title)
    {
    }

    void onInit() override
    {
        m_renderer.pushFeature(std::make_unique<TestFeature>(m_renderer));

        m_renderer.pushFeature(std::make_unique<ImguiFeature>(m_renderer));
    }

    void onTick(float deltaTime) override
    {
        static bool wasPressed = false;
        bool isPressed = Input::isKeyDown(Key::Space);

        if(isPressed && !wasPressed)
        {
            auto* imguiFeature = m_renderer.getFeature<ImguiFeature>();
            if(imguiFeature)
            {
                bool currentState = imguiFeature->isEnabled();
                imguiFeature->setEnabled(!currentState);
                std::println("ImGui is now: {}", !currentState ? "OFF" : "ON");
            }
        }
        wasPressed = isPressed;
    }

    void onImgui() override
    {
        ImGui::Begin("Global");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::End();
    }

private:
};
