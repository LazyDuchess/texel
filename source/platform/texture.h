#pragma once
#if PLATFORM_WII
#include <gccore.h>
#endif
#include <memory>

class Texture{
    public:
        #if PLATFORM_WII
        Texture(std::unique_ptr<GXTexObj> texObj);
        std::unique_ptr<GXTexObj> m_texObj;
        #endif
};