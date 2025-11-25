#pragma once
#include <vector>
#include "mathtypes.h"
#include "aligned_allocator.hpp"

class Mesh {
    public:
        std::vector<Vec3, AlignedAllocator<Vec3, 32>> verts;
        std::vector<Vec3, AlignedAllocator<Vec3, 32>> normals;
        std::vector<Vec2, AlignedAllocator<Vec3, 32>> uvs;
        #if PLATFORM_WII
        void* m_gxDispList;
        int m_gxDispListSize = 0;
        #endif
};