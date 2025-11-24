#pragma once
#if PLATFORM_WII
#include <gccore.h>
#endif

class Texture{
    public:
        #if PLATFORM_WII
        GXTexObj* m_texObj;
        #endif
}