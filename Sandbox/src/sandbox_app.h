#pragma once

#include "application/application.h"
#include "scene/camera_controller.h"

#include <memory>

class SandboxApp : public Application
{
public:
    SandboxApp(uint32_t width, uint32_t height, const std::string& title);

    void onInit() override;

    void onTick(float deltaTime) override;

    void onImgui() override;

    void onInput() override;

private:
    std::unique_ptr<CameraController> m_cameraController;
    // TODO MOCCA:
    // std::unordered_map<std::string, std::shared_ptr<LoadedGLTF>> m_loadedScenes;
};
