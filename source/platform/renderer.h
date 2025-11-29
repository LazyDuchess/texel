#pragma once
#include "texeltypes.h"
#include "mesh.h"
#include "material.h"
#include "glm/glm.hpp"

namespace Renderer{
    extern bool CAPFPS;
    void Initialize();
    void Update();
    void ProcessMesh(Mesh* mesh);
    void QueueMeshRenderCommand(glm::mat4 Matrix, Mesh* mesh, Material* material);
};

class RenderCommand{
    public:
        RenderCommand(glm::mat4 mtx, Mesh* mesh, Material* material);
        glm::mat4 m_matrix;
        Mesh* m_mesh;
        Material* m_material;
};