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

static void LoadRedMesh(Mesh* mesh){
	#include "platform/metalHead.inc"
	Renderer::ProcessMesh(mesh);
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
	gameScene->AddEntity(std::move(ent));
	std::unique_ptr<MeshRenderable> renderable = std::make_unique<MeshRenderable>();
	renderable.get()->m_mesh = testMesh;
	renderable.get()->m_material = testMaterial;

	while (1)
	{
		Renderer::Update();
	}
	return 0;
}