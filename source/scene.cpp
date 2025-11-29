#include "scene.h"
#include "entity.h"

void Scene::SetActiveCamera(Camera* cam){
    m_activeCamera = cam;
}