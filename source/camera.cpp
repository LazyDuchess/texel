#include "camera.h"
#include "glm/gtc/quaternion.hpp"

glm::mat4 Camera::GetViewMatrix(){
    return glm::inverse(GetMatrix());
}