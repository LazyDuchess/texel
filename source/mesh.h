#pragma once
#include <vector>
#include "mathtypes.h"


class Mesh {
    public:
        std::vector<Vec3> verts;
        std::vector<Vec3> normals;
        std::vector<Vec3> colors;
        std::vector<Vec2> uvs;
};