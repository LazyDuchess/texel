#pragma once
#include "renderable.h"

class MeshRenderable : public Renderable {
    public:
        Mesh* m_mesh;
        Material* m_material;
        void QueueRenderCommands();
};