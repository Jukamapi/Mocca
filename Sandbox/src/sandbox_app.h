#pragma once

#include "application/application.h"
#include "core/input.h"
#include "experiments/imgui_feature.h"
#include "experiments/mesh_feature.h"
#include "experiments/test_feature.h"
#include "experiments/triangle_feature.h"
#include "resource/asset_manager.h"
#include "scene/camera_controller.h"
#include "scene/scene.h"


#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

#include <memory>


class SandboxApp : public Application
{
public:
    SandboxApp(uint32_t width, uint32_t height, const std::string& title)
        : Application(width, height, title)
    {
    }

    void onInit() override
    {
        auto structureModel = m_assetManager->loadModel("structure.glb");

        if(structureModel)
        {
            auto structureNode = m_scene->instantiate(structureModel);
            m_scene->addRootNode("structure", structureNode);
        }

        m_scene->getCamera().position = glm::vec3(30.0f, 0.0f, -85.0f);
        m_cameraController = std::make_unique<CameraController>(m_scene->getCamera());


        m_renderer->pushFeature(std::make_unique<TestFeature>(*m_renderer));

        // m_renderer->pushFeature(std::make_unique<TriangleFeature>(*m_renderer));

        m_renderer->pushFeature(std::make_unique<MeshFeature>(*m_renderer, *m_assetManager, *m_scene));

        m_renderer->pushFeature(std::make_unique<ImguiFeature>());
    }

    void onTick(float deltaTime) override
    {
        m_cameraController->update(deltaTime);

        if(Input::isKeyPressed(Key::X))
        {
            if(auto* triangleFeature = m_renderer->getFeature<TriangleFeature>())
            {
                triangleFeature->setEnabled(!triangleFeature->isEnabled());
            }
        }

        static float time = 0.0f;
        time += deltaTime;
        // suzanne spinning
        if(auto suzanne = m_scene->getNode("Suzanne"))
        {
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(2.5f, 0.0f, 0.0f));

            glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.0f, 1.0f, 0.0f));

            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

            suzanne->setLocalTransform(translation * rotation * scale);
        }
    }

    void onImgui() override
    {
        ImGui::Begin("Global");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::End();
    }

    void onInput() override
    {
        if(Input::isKeyPressed(Key::Escape))
        {
            close();
        }
    }

private:
    std::unique_ptr<CameraController> m_cameraController;
    // TODO MOCCA:
    // std::unordered_map<std::string, std::shared_ptr<LoadedGLTF>> m_loadedScenes;
};
