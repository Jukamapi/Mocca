#pragma once

#include "application/application.h"
#include "core/input.h"
#include "experiments/imgui_feature.h"
#include "experiments/mesh_feature.h"
#include "experiments/test_feature.h"
#include "experiments/triangle_feature.h"
#include "resource/asset_manager.h"
#include "scene/mesh_node.h"
#include "scene/scene.h"

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
        auto meshes = m_assetManager->loadGltfMeshes("basicmesh.glb");
        if(meshes)
        {
            auto defaultMaterial = std::make_shared<MaterialInstance>(m_assetManager->getDefaultMaterial());

            for(auto& mesh : *meshes)
            {
                auto node = std::make_shared<MeshNode>();
                node->setMesh(mesh);

                for(auto& surface : mesh->surfaces)
                {
                    surface.material = defaultMaterial;
                }

                m_scene->addRootNode(mesh->name, node);
            }
        }

        m_renderer->pushFeature(std::make_unique<TestFeature>(*m_renderer));

        // m_renderer->pushFeature(std::make_unique<TriangleFeature>(*m_renderer));

        m_renderer->pushFeature(std::make_unique<MeshFeature>(*m_renderer, *m_assetManager, *m_scene));

        m_renderer->pushFeature(std::make_unique<ImguiFeature>());
    }

    void onTick(float deltaTime) override
    {
        static float time = 0.0f;
        time += deltaTime;

        // toggling triangle
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

        // suzanne spinning
        if(auto suzanne = m_scene->getNode("Suzanne"))
        {
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(2.5f, 0.0f, 0.0f));

            glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.0f, 1.0f, 0.0f));

            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

            suzanne->setLocalTransform(translation * rotation * scale);
        }

        Camera& camera = m_scene->getCamera();

        if(Input::isMouseButtonDown(MouseButton::Right))
        {
            camera.yaw += (float)Input::mouseDeltaX * camera.mouseSensitivity;
            camera.pitch -= (float)Input::mouseDeltaY * camera.mouseSensitivity;
            camera.pitch = glm::clamp(camera.pitch, -89.0f, 89.0f);
        }

        const float speed = camera.moveSpeed * deltaTime;
        glm::mat4 camRot = camera.getRotationMatrix();
        glm::vec3 forward = -glm::vec3(camRot[2]);
        glm::vec3 right = glm::vec3(camRot[0]);

        if(Input::isKeyDown(Key::W))
            camera.position += forward * speed;
        if(Input::isKeyDown(Key::S))
            camera.position -= forward * speed;
        if(Input::isKeyDown(Key::D))
            camera.position += right * speed;
        if(Input::isKeyDown(Key::A))
            camera.position -= right * speed;
    }

    void onImgui() override
    {
        ImGui::Begin("Global");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::End();
    }

private:
};
