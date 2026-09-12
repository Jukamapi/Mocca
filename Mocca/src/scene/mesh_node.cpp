#include "mesh_node.h"

void MeshNode::draw(const glm::mat4& topMatrix, DrawContext& ctx)
{
    if(m_mesh)
    {
        glm::mat4 nodeMatrix = topMatrix * m_worldTransform;

        for(const auto& surface : m_mesh->surfaces)
        {
            RenderObject object{
                .indexCount = surface.count,
                .firstIndex = surface.startIndex,
                .indexBuffer = m_mesh->meshBuffers.indexBuffer.getBuffer(),
                .material = surface.material.get(),
                .transform = nodeMatrix,
                .vertexBufferAddress = m_mesh->meshBuffers.vertexBufferAddress
            };

            if(surface.material && surface.material->passType == MaterialPass::Transparent)
            {
                ctx.transparentSurfaces.push_back(object);
            }
            else
            {
                ctx.opaqueSurfaces.push_back(object);
            }
        }
    }

    Node::draw(topMatrix, ctx);
}