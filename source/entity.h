#pragma once
#include <vector>
#include <memory>
#include "component.h"
#include "renderable.h"
#include "glm/glm.hpp"
#include "glm/ext/quaternion_float.hpp"

class Scene;

class Entity {
    public:
        glm::vec3 m_scale;
        glm::vec3 m_position;
        glm::quat m_rotation;
        Scene* m_scene;
        std::vector<Renderable*> m_renderables;
        std::vector<std::unique_ptr<Component>> m_components;
        void AddComponent(std::unique_ptr<Component> component);
        glm::mat4 GetMatrix();
};