#pragma once
#include "mathtypes.h"
#include "mesh.h"
#include "material.h"

namespace Renderer{
    void Initialize();
    void Update();
};

class RenderCommand{
    public:
    Matrix m_matrix;
    Mesh* m_mesh;
    Material* m_material;
};