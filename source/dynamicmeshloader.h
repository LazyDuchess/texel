#pragma once
#include "mesh.h"

namespace DynamicMeshLoader
{
    bool LoadFromFile(Mesh* mesh, const char* path);
}