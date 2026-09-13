#pragma once

#include "camera.h"
#include "renderer/draw_types.h"
#include "scene/node.h"


#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class GlobalRenderData;

class Scene
{
public:
    void addRootNode(const std::string& name, std::shared_ptr<Node> node);
    std::shared_ptr<Node> getNode(const std::string& name);

    void update();

    const DrawContext& getDrawContext() const
    {
        return m_drawContext;
    }

    Camera& getCamera()
    {
        return m_camera;
    }
    const Camera& getCamera() const
    {
        return m_camera;
    }

    GlobalRenderData getRenderData(float aspectRatio) const;

private:
    std::vector<std::shared_ptr<Node>> m_rootNodes;
    std::unordered_map<std::string, std::shared_ptr<Node>> m_nodeRegistry;
    DrawContext m_drawContext;
    Camera m_camera;
};