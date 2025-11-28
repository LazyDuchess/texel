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
#include "texeltypes.h"

static float timer = 0.0f;
static int TransformMode = 9;
static Entity* testEntity;

static void LoadRedMesh(Mesh* mesh){
	#include "platform/metalHead.inc"
	Renderer::ProcessMesh(mesh);
}

static void UpdateTransform(){
	testEntity->m_position = {0.0f, 0.0f, 0.0f};
	testEntity->m_scale = {1.0f, 1.0f, 1.0f};
	testEntity->m_rotation = glm::identity<glm::quat>();
	float delta = sin(timer) * 2.0f;
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

void CameraControls(Camera* cam){
	const float cameraSpeed = 0.1f;
	const float cameraSpinSpeed = 0.05f;
	u32 heldButtons = WPAD_ButtonsHeld(0);
	glm::quat rot = cam->m_rotation;

	glm::vec3 fw = rot * VECTOR_FORWARD;
	glm::vec3 rg = rot * VECTOR_RIGHT;
	glm::vec3 up = rot * VECTOR_UP;

	if (heldButtons & WPAD_BUTTON_A){
		if (heldButtons & WPAD_BUTTON_UP){
			cam->m_rotation = glm::normalize(
    			glm::angleAxis(-cameraSpinSpeed, glm::normalize(rg)) * cam->m_rotation
			);
		}
		if (heldButtons & WPAD_BUTTON_DOWN){
			cam->m_rotation = glm::normalize(
    			glm::angleAxis(cameraSpinSpeed, glm::normalize(rg)) * cam->m_rotation
			);
		}
	}
	else
	{
		if (heldButtons & WPAD_BUTTON_UP){
			cam->m_position += fw * cameraSpeed;
		}
		if (heldButtons & WPAD_BUTTON_DOWN){
			cam->m_position -= fw * cameraSpeed;
		}
	}
	if (heldButtons & WPAD_BUTTON_RIGHT){
		cam->m_rotation = glm::normalize(
    		glm::angleAxis(-cameraSpinSpeed, VECTOR_UP) * cam->m_rotation
		);
	}
	if (heldButtons & WPAD_BUTTON_LEFT){
		cam->m_rotation = glm::normalize(
    		glm::angleAxis(cameraSpinSpeed, VECTOR_UP) * cam->m_rotation
		);
	}
	if (heldButtons & WPAD_BUTTON_PLUS){
		cam->m_position += VECTOR_UP * cameraSpeed;
	}
	if (heldButtons & WPAD_BUTTON_MINUS){
		cam->m_position -= VECTOR_UP * cameraSpeed;
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
	std::unique_ptr<Camera> camEnt = std::make_unique<Camera>();
	Camera* cam = camEnt.get();
	std::unique_ptr<MeshRenderable> renderable = std::make_unique<MeshRenderable>();
	renderable.get()->m_mesh = testMesh;
	renderable.get()->m_material = testMaterial;
	testEntity->AddComponent(std::move(renderable));
	gameScene->AddEntity(std::move(ent));
	gameScene->AddEntity(std::move(camEnt));
	gameScene->m_activeCamera = cam;

	while (1)
	{
		timer += 0.1f;
		CameraControls(cam);
		UpdateTransform();
		WPAD_ScanPads();
		u32 buttons = WPAD_ButtonsDown(0);
		if (buttons & WPAD_BUTTON_HOME) exit(0);
		if (buttons & WPAD_BUTTON_B){
			TransformMode++;
			if (TransformMode > 9)
				TransformMode = 0;
		}
		Renderer::Update();
	}
	return 0;
}