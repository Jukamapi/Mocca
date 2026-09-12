#include "scene/node.h"

void Node::addChild(std::shared_ptr<Node> child)
{
    child->m_parent = weak_from_this();
    child->m_isDirty = true;
    m_children.push_back(std::move(child));
}

void Node::setLocalTransform(const glm::mat4& matrix)
{
    m_localTransform = matrix;
    m_isDirty = true;
}

void Node::updateTransforms(const glm::mat4& parentMatrix)
{
    if(m_isDirty)
    {
        m_worldTransform = parentMatrix * m_localTransform;
        m_isDirty = false;
    }

    for(auto& child : m_children)
    {
        child->updateTransforms(m_worldTransform);
    }
}

void Node::draw(const glm::mat4& topMatrix, DrawContext& ctx)
{
    for(auto& child : m_children)
    {
        child->draw(topMatrix, ctx);
    }
}