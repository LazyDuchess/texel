#pragma once
#include <vector>
#include <memory>
#include "entity.h"

class Scene {
    public:
        std::vector<std::unique_ptr<Entity>> m_entities;
        void AddEntity(std::unique_ptr<Entity> entity);
};