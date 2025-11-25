#pragma once
#include "platform/texture.h"
#define SHADER_NONE 0
#define SHADER_UNLIT_TEXTURED 1
typedef char shader_id;

class Material{
    public:
        shader_id m_shader;
        Texture* m_texture;
};