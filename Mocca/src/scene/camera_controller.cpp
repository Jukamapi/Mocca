#include "camera_controller.h"

#include "core/input.h"

#include <glm/gtx/norm.hpp>

CameraController::CameraController(Camera& camera)
    : m_camera(camera),
      m_speed(camera.moveSpeed),
      m_sensitivity(camera.mouseSensitivity)
{
}

void CameraController::update(float deltaTime)
{
    if(Input::isMouseButtonDown(MouseButton::Right))
    {
        glm::vec2 delta = Input::getMouseDelta();
        m_camera.yaw += delta.x * m_sensitivity;
        m_camera.pitch -= delta.y * m_sensitivity;
        m_camera.pitch = glm::clamp(m_camera.pitch, -89.0f, 89.0f);
    }

    glm::mat4 rotation = m_camera.getRotationMatrix();
    glm::vec3 forward = -glm::vec3(rotation[2]);
    glm::vec3 right = glm::vec3(rotation[0]);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 moveDir{0.0f};

    if(Input::isKeyDown(Key::W))
        moveDir += forward;
    if(Input::isKeyDown(Key::S))
        moveDir -= forward;
    if(Input::isKeyDown(Key::D))
        moveDir += right;
    if(Input::isKeyDown(Key::A))
        moveDir -= right;
    if(Input::isKeyDown(Key::Space))
        moveDir += up;
    if(Input::isKeyDown(Key::LeftCtrl) || Input::isKeyDown(Key::X))
        moveDir -= up;

    if(glm::length2(moveDir) > 0.0001f)
    {
        m_camera.position += glm::normalize(moveDir) * (m_speed * deltaTime);
    }
}