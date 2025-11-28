#pragma once
#include <vector>
#include <memory>
#include "entity.h"
#include "camera.h"

class Scene {
    public:
        std::vector<std::unique_ptr<Entity>> m_entities;
        Camera* m_activeCamera;
        void AddEntity(std::unique_ptr<Entity> entity);
        void SetActiveCamera(Camera* cam);
};