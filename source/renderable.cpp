#include "renderable.h"
#include "entity.h"

void Renderable::OnAdd(){
    m_owner->m_renderables.push_back(this);
}