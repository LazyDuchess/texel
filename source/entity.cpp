#include "entity.h"
#include "glm/gtc/quaternion.hpp"

void Entity::AddComponent(std::unique_ptr<Component> component){
    Component* componentPointer = component.get();
    m_components.push_back(std::move(component));
    componentPointer->m_owner = this;
    componentPointer->OnAdd();
}

glm::mat4 Entity::GetMatrix(){
    glm::mat4 tf = glm::mat4(1.0f);

    tf = glm::translate(tf, m_position);
    tf *= glm::mat4_cast(m_rotation);
    tf = glm::scale(tf, m_scale);
    return tf;
}