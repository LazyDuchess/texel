#pragma once
#include "platform/texture.h"
#define SHADER_UNLIT 0
typedef char shader_id;

class Material{
    public:
        shader_id m_shader;
        Texture* m_texture;
};