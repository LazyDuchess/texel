#include "entity.h"
#include "glm/gtc/quaternion.hpp"

void Entity::AddComponent(std::unique_ptr<Component> component){
    Component* componentPointer = component.get();
    m_components.push_back(std::move(component));
    componentPointer->m_owner = this;
    componentPointer->OnAdd();
}

glm::mat4 Entity::GetMatrix(){
    glm::mat4 mtx = glm::mat4_cast(m_rotation);
    mtx[0] *= m_scale.x;
    mtx[1] *= m_scale.y;
    mtx[2] *= m_scale.z;
    mtx[3] = glm::vec4(m_position, 1.0f);
    return mtx;
}