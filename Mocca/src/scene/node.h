#pragma once

#include "renderable.h"

#include <memory>

class Node : public IRenderable, public std::enable_shared_from_this<Node>
{
public:
    virtual ~Node() = default;

    void addChild(std::shared_ptr<Node> child);
    bool removeChild(const std::shared_ptr<Node>& child);

    void setLocalTransform(const glm::mat4& matrix);

    const glm::mat4& getLocalTransform() const
    {
        return m_localTransform;
    }
    const glm::mat4& getWorldTransform() const
    {
        return m_worldTransform;
    }

    void updateTransforms(const glm::mat4& parentMatrix = glm::mat4{1.0f}, bool parentDirty = false);

    void draw(DrawContext& ctx) override;

    bool getIsDirty() const
    {
        return m_isDirty;
    }

    std::weak_ptr<Node> getParent() const
    {
        return m_parent;
    }

    bool hasParent() const
    {
        return !m_parent.expired();
    }

protected:
    std::weak_ptr<Node> m_parent;
    std::vector<std::shared_ptr<Node>> m_children;

    glm::mat4 m_localTransform{1.0f};
    glm::mat4 m_worldTransform{1.0f};

    bool m_isDirty{true};
};