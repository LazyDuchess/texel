#include "camera.h"
#include "glm/gtc/quaternion.hpp"

glm::mat4 Camera::GetViewMatrix(){
    glm::mat4 view = glm::mat4(1.0f);

    view = glm::translate(view, m_position);
    view *= glm::mat4_cast(m_rotation);

    view = glm::inverse(view);

    return view;
}