#include "app.h"
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
#include "time.h"

namespace App {
    static float timer = 0.0f;
    static int TransformMode = 9;
    static Entity* testEntity;
    static Camera* cam;

    static void LoadRedMesh(Mesh* mesh){
        #include "testapp/metalHead.inc"
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
        const float cameraSpeed = 3.0f;
        const float cameraSpinSpeed = 1.0f;
        const float fovSpeed = 40.0f;
        u32 heldButtons = WPAD_ButtonsHeld(0);
        u32 downButtons = WPAD_ButtonsDown(0);
        glm::quat rot = cam->m_rotation;

        glm::vec3 fw = rot * VECTOR_FORWARD;
        glm::vec3 rg = rot * VECTOR_RIGHT;
        glm::vec3 up = rot * VECTOR_UP;

        if (heldButtons & WPAD_BUTTON_A){
            if (heldButtons & WPAD_BUTTON_UP){
                cam->m_rotation = glm::normalize(
                    glm::angleAxis(cameraSpinSpeed * Time::DeltaTime, glm::normalize(rg)) * cam->m_rotation
                );
            }
            if (heldButtons & WPAD_BUTTON_DOWN){
                cam->m_rotation = glm::normalize(
                    glm::angleAxis(-cameraSpinSpeed * Time::DeltaTime, glm::normalize(rg)) * cam->m_rotation
                );
            }
        }
        else
        {
            if (heldButtons & WPAD_BUTTON_UP){
                cam->m_position += fw * cameraSpeed * Time::DeltaTime;
            }
            if (heldButtons & WPAD_BUTTON_DOWN){
                cam->m_position -= fw * cameraSpeed * Time::DeltaTime;
            }
        }
        if (heldButtons & WPAD_BUTTON_RIGHT){
            cam->m_rotation = glm::normalize(
                glm::angleAxis(-cameraSpinSpeed * Time::DeltaTime, VECTOR_UP) * cam->m_rotation
            );
        }
        if (heldButtons & WPAD_BUTTON_LEFT){
            cam->m_rotation = glm::normalize(
                glm::angleAxis(cameraSpinSpeed * Time::DeltaTime, VECTOR_UP) * cam->m_rotation
            );
        }
        if (heldButtons & WPAD_BUTTON_PLUS){
            cam->m_fieldOfView += fovSpeed * Time::DeltaTime;
            if (cam->m_fieldOfView >= 100)
                cam->m_fieldOfView = 100;
        }
        if (heldButtons & WPAD_BUTTON_MINUS){
            cam->m_fieldOfView -= fovSpeed * Time::DeltaTime;
            if (cam->m_fieldOfView <= 1)
                cam->m_fieldOfView = 1;
        }
        if (downButtons & WPAD_BUTTON_1)
            Renderer::CAPFPS = !Renderer::CAPFPS;
    }

    void Initialize(){
        TPLFile texturesTPL;
	    std::unique_ptr<GXTexObj> textTexture = std::make_unique<GXTexObj>();
	    TPL_OpenTPLFromMemory(&texturesTPL, (void *)textures_tpl,textures_tpl_size);
	    TPL_GetTexture(&texturesTPL,metalhead,textTexture.get());

	    Material* testMaterial = new Material();
	    testMaterial->m_shader = SHADER_UNLIT_TEXTURED;
	    testMaterial->m_texture = new Texture(std::move(textTexture));

	    Mesh* testMesh = new Mesh();
	    LoadRedMesh(testMesh);

	    Scene* gameScene = new Scene();
	    SceneManager::CurrentScene = gameScene;

	    // Red character ent
	    testEntity = gameScene->AddEntity<Entity>();
	    MeshRenderable* meshRenderer = testEntity->AddComponent<MeshRenderable>();
	    meshRenderer->m_mesh = testMesh;
	    meshRenderer->m_material = testMaterial;

	    // Camera ent
	    cam = gameScene->AddEntity<Camera>();
	    cam->m_position.z = 2.0f;
	    cam->m_position.y = 1.0f;

	    // Set as active camera
	    gameScene->m_activeCamera = cam;

    }
    void FrameUpdate(){
        timer += Time::DeltaTime * 2.0;
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
    }
}