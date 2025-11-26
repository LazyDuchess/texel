#pragma once
#include <cstdint>
#include <memory>
#include <cstdlib>
typedef float f32;

struct Vec2 {
    f32 x, y;
};

struct Vec3 {
    f32 x, y, z;
};

typedef f32	Matrix[3][4];

typedef uint16_t ui16;