#pragma once
#include "entity.h"

class Camera : public Entity {
    public:
        float m_fieldOfView = 60.0f;
        float m_nearPlane = 0.1f;
        float m_farPlane = 100.0f;
        glm::mat4 GetViewMatrix();
};