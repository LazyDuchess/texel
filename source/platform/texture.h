#pragma once
#if PLATFORM_WII
#include <gccore.h>
#endif

class Texture{
    public:
        #if PLATFORM_WII
        Texture(GXTexObj* texObj);
        GXTexObj* m_texObj;
        #endif
};