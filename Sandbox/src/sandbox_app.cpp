#include "sandbox_app.h"

#include "core/input.h"
#include "experiments/imgui_feature.h"
#include "experiments/mesh_feature.h"
#include "experiments/test_feature.h"
#include "experiments/triangle_feature.h"
#include "renderer/renderer.h"
#include "resource/asset_manager.h"
#include "scene/scene.h"


#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

SandboxApp::SandboxApp(uint32_t width, uint32_t height, const std::string& title)
    : Application(width, height, title)
{
}

void SandboxApp::onInit()
{
    auto structureModel = m_assetManager->loadModel("structure.glb");

    if(structureModel)
    {
        auto structureNode = m_scene->instantiate(structureModel);
        m_scene->addRootNode("structure", structureNode);
    }

    auto helmetModel = m_assetManager->loadModel("DamagedHelmet.glb");

    if(helmetModel)
    {
        auto helmetNode = m_scene->instantiate(helmetModel);
        m_scene->addRootNode("helmet", helmetNode);
    }

    m_scene->getCamera().position = glm::vec3(1.0f, 0.0f, 0.0f);
    m_cameraController = std::make_unique<CameraController>(m_scene->getCamera());


    m_renderer->pushFeature(std::make_unique<TestFeature>(*m_renderer));

    // m_renderer->pushFeature(std::make_unique<TriangleFeature>(*m_renderer));

    m_renderer->pushFeature(std::make_unique<MeshFeature>(*m_renderer, *m_assetManager, *m_scene));

    m_renderer->pushFeature(std::make_unique<ImguiFeature>());
}

void SandboxApp::onTick(float deltaTime)
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
    // helm spinning
    if(auto helmet = m_scene->getNode("helmet"))
    {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(2.5f, 0.0f, 0.0f));

        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

        helmet->setLocalTransform(translation * rotation * scale);
    }
}

void SandboxApp::onImgui()
{
    ImGui::Begin("Global");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::End();
}

void SandboxApp::onInput()
{
    if(Input::isKeyPressed(Key::Escape))
    {
        close();
    }
}