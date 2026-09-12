#pragma once

#include "renderer/draw_types.h"
#include "scene/node.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

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

private:
    std::vector<std::shared_ptr<Node>> m_rootNodes;
    std::unordered_map<std::string, std::shared_ptr<Node>> m_nodeRegistry;
    DrawContext m_drawContext;
};