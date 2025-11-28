#include "scene.h"
#include "entity.h"

void Scene::AddEntity(std::unique_ptr<Entity> entity){
    entity.get()->m_scene = this;
    m_entities.push_back(std::move(entity));
}

void Scene::SetActiveCamera(Camera* cam){
    m_activeCamera = cam;
}