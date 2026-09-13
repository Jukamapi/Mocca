#include "scene/node.h"

void Node::addChild(std::shared_ptr<Node> child)
{
    if(!child)
        return;

    if(auto oldParent = child->m_parent.lock())
    {
        oldParent->removeChild(child);
    }

    child->m_parent = weak_from_this();
    child->m_isDirty = true;
    m_children.push_back(std::move(child));
}

bool Node::removeChild(const std::shared_ptr<Node>& child)
{
    if(!child)
        return false;

    auto it = std::find(m_children.begin(), m_children.end(), child);

    if(it != m_children.end())
    {
        (*it)->m_parent.reset();
        (*it)->m_isDirty = true;

        m_children.erase(it);
        return true;
    }

    return false;
}

void Node::setLocalTransform(const glm::mat4& matrix)
{
    m_localTransform = matrix;
    m_isDirty = true;
}

void Node::updateTransforms(const glm::mat4& parentMatrix, bool parentDirty)
{
    bool isDirty = m_isDirty || parentDirty;

    if(isDirty)
    {
        m_worldTransform = parentMatrix * m_localTransform;
        m_isDirty = false;
    }

    for(auto& child : m_children)
    {
        child->updateTransforms(m_worldTransform, isDirty);
    }
}

void Node::draw(DrawContext& ctx)
{
    for(auto& child : m_children)
    {
        child->draw(ctx);
    }
}