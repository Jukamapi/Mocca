#pragma once

#include "scene/camera.h"

class CameraController
{
public:
    explicit CameraController(Camera& camera);

    void update(float deltaTime);

    void setMovementSpeed(float speed)
    {
        m_speed = speed;
    }
    void setSensitivity(float sens)
    {
        m_sensitivity = sens;
    }

private:
    Camera& m_camera;
    float m_speed = 5.0f;
    float m_sensitivity = 0.1f;
};