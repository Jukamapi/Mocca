#include "mesh_node.h"

void MeshNode::draw(DrawContext& ctx)
{
    if(m_mesh)
    {
        for(const auto& surface : m_mesh->surfaces)
        {
            RenderObject object{
                .indexCount = surface.count,
                .firstIndex = surface.startIndex,
                .indexBuffer = m_mesh->meshBuffers.indexBuffer.getBuffer(),
                .material = surface.material.get(),
                .transform = m_worldTransform,
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

    Node::draw(ctx);
}