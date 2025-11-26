#pragma once
#include <vector>
#include "texeltypes.h"
#include "aligned_allocator.hpp"

class Mesh {
    public:
        std::vector<Vec3, AlignedAllocator<Vec3, 32>> verts;
        std::vector<Vec3, AlignedAllocator<Vec3, 32>> normals;
        std::vector<Vec2, AlignedAllocator<Vec2, 32>> uvs;
        std::vector<ui16, AlignedAllocator<ui16, 32>> indices;
        #if PLATFORM_WII
        void* m_gxDispList;
        int m_gxDispListSize = 0;
        #endif
};