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
        glm::vec3 m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::quat m_rotation = glm::identity<glm::quat>();
        Scene* m_scene;
        Renderable* m_renderable;
        std::vector<std::unique_ptr<Component>> m_components;
        void AddComponent(std::unique_ptr<Component> component);
        glm::mat4 GetMatrix();
};