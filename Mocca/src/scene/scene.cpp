#include "scene.h"

#include "mesh_node.h"
#include "resource/model_asset.h"


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
        root->updateTransforms();
        root->draw(m_drawContext);
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

std::shared_ptr<Node> Scene::instantiate(const std::shared_ptr<ModelAsset>& model, bool registerNamedNodes)
{
    if(!model)
        return nullptr;

    std::vector<std::shared_ptr<Node>> sceneNodes;
    sceneNodes.reserve(model->nodes.size());

    for(const auto& desc : model->nodes)
    {
        std::shared_ptr<Node> node;

        if(desc.meshIndex.has_value())
        {
            auto meshNode = std::make_shared<MeshNode>();
            meshNode->setMesh(model->meshes[*desc.meshIndex]);
            node = meshNode;
        }
        else
        {
            node = std::make_shared<Node>();
        }

        node->setLocalTransform(desc.localTransform);
        sceneNodes.push_back(node);

        // register node by name
        if(registerNamedNodes && !desc.name.empty())
        {
            m_nodeRegistry[desc.name] = node;
        }
    }

    for(size_t i = 0; i < model->nodes.size(); ++i)
    {
        for(uint32_t childIdx : model->nodes[i].children)
        {
            sceneNodes[i]->addChild(sceneNodes[childIdx]);
        }
    }

    auto modelRoot = std::make_shared<Node>();
    for(const auto& node : sceneNodes)
    {
        if(!node->hasParent())
        {
            modelRoot->addChild(node);
        }
    }

    modelRoot->updateTransforms();

    return modelRoot;
}