#pragma once
#include <vector>
typedef float f32;

class Mesh {
    public:
        std::vector<f32[3]> verts;
        std::vector<f32[3]> colors;
        std::vector<f32[2]> uvs;
};