#include "camera.h"


// TODO: refresh knowledge about matrixes used in graphics, and quaternions
glm::mat4 Camera::getViewMatrix() const
{

    glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 rotation = getRotationMatrix();

    return glm::inverse(translation * rotation);
}

glm::mat4 Camera::getRotationMatrix() const
{

    glm::quat pitchRotation = glm::angleAxis(glm::radians(pitch), glm::vec3{1.f, 0.f, 0.f});
    glm::quat yawRotation = glm::angleAxis(glm::radians(yaw), glm::vec3{0.f, -1.f, 0.f});

    return glm::toMat4(yawRotation) * glm::toMat4(pitchRotation);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const
{

    glm::mat4 proj = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);

    proj[1][1] *= -1.0f;

    return proj;
}

void Camera::update()
{
    glm::mat4 cameraRotation = getRotationMatrix();
    position += glm::vec3(cameraRotation * glm::vec4(velocity * 0.5f, 0.f));
}
