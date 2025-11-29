#pragma once
#include <vector>
#include <memory>
#include "entity.h"
#include "camera.h"

class Scene {
    public:
        std::vector<std::unique_ptr<Entity>> m_entities;
        Camera* m_activeCamera;
        template<typename T, typename... Args>
        T* AddEntity(Args&&... args){
            auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
            T* raw = ptr.get();
            ((Entity*)raw)->m_scene = this;
            m_entities.push_back(std::move(ptr));
            return raw;
        }
        void SetActiveCamera(Camera* cam);
};