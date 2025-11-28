#include "mesh_renderable.h"
#include "platform/renderer.h"
#include "entity.h"

void MeshRenderable::QueueRenderCommands(){
    Renderer::QueueMeshRenderCommand(m_owner->GetMatrix(), m_mesh, m_material);
}