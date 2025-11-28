// adapted from	the	original acube demo	by tkcne.

// enjoy

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <platform/renderer.h>
#include "scene.h"
#include "scene_manager.h"
#include "mesh.h"
#include "platform/texture.h"
#include "material.h"
#include "textures_tpl.h"
#include "textures.h"
#include "mesh_renderable.h"
#include "glm/gtc/quaternion.hpp"

static float timer = 0.0f;
static int TransformMode = 0;
static Entity* testEntity;

static void LoadRedMesh(Mesh* mesh){
	#include "platform/metalHead.inc"
	Renderer::ProcessMesh(mesh);
}

static void UpdateTransform(){
	testEntity->m_position = {0.0f, 0.0f, 0.0f};
	testEntity->m_scale = {1.0f, 1.0f, 1.0f};
	testEntity->m_rotation = glm::identity<glm::quat>();
	float delta = sin(timer) * 5.0f;
	switch(TransformMode){
		case 0:
		testEntity->m_scale.x = delta;
		break;

		case 1:
		testEntity->m_scale.y = delta;
		break;

		case 2:
		testEntity->m_scale.z = delta;
		break;

		case 3:
		testEntity->m_position.x = delta;
		break;

		case 4:
		testEntity->m_position.y = delta;
		break;

		case 5:
		testEntity->m_position.z = delta;
		break;

		case 6:
		testEntity->m_rotation = glm::quat({timer, 0.0f, 0.0f});
		break;

		case 7:
		testEntity->m_rotation = glm::quat({0.0f, timer, 0.0f});
		break;

		case 8:
		testEntity->m_rotation = glm::quat({0.0f, 0.0f, timer});
		break;
	}
	
}

int	main(void) {
	Renderer::Initialize();
	
	Scene* gameScene = new Scene();
	SceneManager::currentScene = gameScene;

	TPLFile texturesTPL;
	std::unique_ptr<GXTexObj> textTexture = std::make_unique<GXTexObj>();
	TPL_OpenTPLFromMemory(&texturesTPL, (void *)textures_tpl,textures_tpl_size);
	TPL_GetTexture(&texturesTPL,metalhead,textTexture.get());

	Material* testMaterial = new Material();
	testMaterial->m_shader = SHADER_UNLIT_TEXTURED;
	testMaterial->m_texture = new Texture(std::move(textTexture));

	Mesh* testMesh = new Mesh();
	LoadRedMesh(testMesh);

	std::unique_ptr<Entity> ent = std::make_unique<Entity>();
	testEntity = ent.get();
	std::unique_ptr<MeshRenderable> renderable = std::make_unique<MeshRenderable>();
	renderable.get()->m_mesh = testMesh;
	renderable.get()->m_material = testMaterial;
	testEntity->AddComponent(std::move(renderable));
	gameScene->AddEntity(std::move(ent));

	while (1)
	{
		Renderer::Update();
		timer += 0.1f;
		UpdateTransform();
		WPAD_ScanPads();
		u32 buttons = WPAD_ButtonsDown(0);
		if (buttons & WPAD_BUTTON_HOME) exit(0);
		if (buttons & WPAD_BUTTON_B){
			TransformMode++;
			if (TransformMode > 8)
				TransformMode = 0;
		}
	}
	return 0;
}