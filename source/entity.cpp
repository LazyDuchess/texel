#include "entity.h"
#include "glm/gtc/quaternion.hpp"

glm::mat4 Entity::GetMatrix(){
    glm::mat4 tf = glm::mat4(1.0f);

    tf = glm::translate(tf, m_position);
    tf *= glm::mat4_cast(m_rotation);
    tf = glm::scale(tf, m_scale);
    return tf;
}