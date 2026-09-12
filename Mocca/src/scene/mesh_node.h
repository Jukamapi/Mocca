#pragma once

#include "node.h"
#include "resource/mesh_types.h"

class MeshNode : public Node
{
public:
    void setMesh(std::shared_ptr<MeshAsset> mesh)
    {
        m_mesh = std::move(mesh);
    }
    const std::shared_ptr<MeshAsset>& getMesh() const
    {
        return m_mesh;
    }

    void draw(const glm::mat4& topMatrix, DrawContext& ctx) override;

private:
    std::shared_ptr<MeshAsset> m_mesh;
};