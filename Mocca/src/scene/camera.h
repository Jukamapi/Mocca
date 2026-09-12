#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>


class Camera
{
public:
    Camera() = default;

    glm::vec3 velocity;
    glm::vec3 position{0.0f, 0.0f, 5.0f};

    float pitch{0.f};
    float yaw{0.f};

    float moveSpeed{5.0f};
    float mouseSensitivity{0.2f};
    float fov{70.0f};

    float nearPlane{10000.0f};
    float farPlane{0.1f};

    glm::mat4 getViewMatrix() const;
    glm::mat4 getRotationMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;

    void update();
};