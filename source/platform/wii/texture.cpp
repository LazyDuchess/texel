#include "platform/texture.h"

Texture::Texture(std::unique_ptr<GXTexObj> texObj){
    m_texObj = std::move(texObj);
}