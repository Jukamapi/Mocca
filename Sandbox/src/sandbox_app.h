#pragma once

#include "application/application.h"
#include "core/input.h"
#include "experiments/imgui_feature.h"
#include "experiments/mesh_feature.h"
#include "experiments/test_feature.h"
#include "experiments/triangle_feature.h"
#include "resource/asset_manager.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

#include <memory>
#include <print>


class SandboxApp : public Application
{
public:
    SandboxApp(uint32_t width, uint32_t height, const std::string& title)
        : Application(width, height, title)
    {
    }

    void onInit() override
    {
        m_loadedMeshes = m_assetManager->loadGltfMeshes("basicmesh.glb").value();

        m_renderer->pushFeature(std::make_unique<TestFeature>(*m_renderer));

        m_renderer->pushFeature(std::make_unique<TriangleFeature>(*m_renderer));

        m_renderer->pushFeature(std::make_unique<MeshFeature>(*m_renderer, &m_loadedMeshes));

        m_renderer->pushFeature(std::make_unique<ImguiFeature>());
    }

    void onTick(float deltaTime) override
    {
        static bool wasPressed = false;
        bool isPressed = Input::isKeyDown(Key::Space);

        if(isPressed && !wasPressed)
        {
            auto* triangleFeature = m_renderer->getFeature<TriangleFeature>();
            if(triangleFeature)
            {
                bool currentState = triangleFeature->isEnabled();
                triangleFeature->setEnabled(!currentState);
                std::println("Triangle is now: {}", !currentState ? "ON" : "OFF");
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
    std::vector<std::shared_ptr<MeshAsset>> m_loadedMeshes;
};
