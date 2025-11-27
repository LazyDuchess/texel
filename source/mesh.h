#pragma once
#include <vector>
#include "texeltypes.h"
#include "aligned_allocator.hpp"
#include "glm/glm.hpp"

class Mesh {
    public:
        std::vector<glm::vec3, AlignedAllocator<glm::vec3, 32>> verts;
        std::vector<glm::vec3, AlignedAllocator<glm::vec3, 32>> normals;
        std::vector<glm::vec2, AlignedAllocator<glm::vec2, 32>> uvs;
        std::vector<ui16, AlignedAllocator<ui16, 32>> indices;
        #if PLATFORM_WII
        std::vector<char, AlignedAllocator<char, 32>> m_gxDispList;
        int m_gxDispListSize = 0;
        #endif
};