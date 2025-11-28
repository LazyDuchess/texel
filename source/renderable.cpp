#include "renderable.h"
#include "entity.h"

void Renderable::OnAdd(){
    m_owner->m_renderable = this;
}

void Renderable::OnRemove(){
    m_owner->m_renderable = nullptr;
}