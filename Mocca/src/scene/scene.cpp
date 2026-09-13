#include "scene.h"

void Scene::addRootNode(const std::string& name, std::shared_ptr<Node> node)
{
    m_nodeRegistry[name] = node;
    m_rootNodes.push_back(node);
}

std::shared_ptr<Node> Scene::getNode(const std::string& name)
{
    auto it = m_nodeRegistry.find(name);
    return (it != m_nodeRegistry.end()) ? it->second : nullptr;
}

void Scene::update()
{
    m_drawContext.clear();

    for(auto& root : m_rootNodes)
    {
        root->updateTransforms(glm::mat4{1.0f});
        root->draw(glm::mat4{1.0f}, m_drawContext);
    }
}

GlobalRenderData Scene::getRenderData(float aspectRatio) const
{
    glm::mat4 view = m_camera.getViewMatrix();
    glm::mat4 proj = m_camera.getProjectionMatrix(aspectRatio);

    return GlobalRenderData{
        .view = view,
        .proj = proj,
        .viewproj = proj * view,
        .ambientColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f),
        .sunlightDirection = glm::normalize(glm::vec4(0.5f, 1.0f, 0.5f, 1.0f)),
        .sunlightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.5f)
    };
}