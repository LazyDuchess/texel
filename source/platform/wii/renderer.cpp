#define	FIFO_SIZE (256*1024)
#define DISPLIST_SIZE 64000
#include "platform/renderer.h"
#include "mesh.h"
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <stdio.h>

#include "scene_manager.h"
#include "scene.h"
#include "entity.h"
#include "renderable.h"
#include <cassert>
#include "camera.h"
#include "ogc/lwp_watchdog.h"
#include <fat.h>

RenderCommand::RenderCommand(glm::mat4 mtx, Mesh* mesh, Material* material){
	m_matrix = mtx;
	m_mesh = mesh;
	m_material = material;
}

namespace Renderer{
	bool CAPFPS = false;
    static GXRModeObj	*screenMode;
    static void	*frameBuffer;
    static vu8	readyForCopy;

	static Mtx44	projection;
	static Mtx	view;

    static void update_screen(	Scene* scene, Mtx	viewMatrix );
    static void	copy_buffers(u32 unused);

	static void *tempDisplayList;

	static s8 HWButton = -1;

	static uint64_t startDeltaTime = 0;

	/**
	 * Callback for the reset button on the Wii.
	 */
	static void WiiResetPressed(u32 irq, void* ctx)
	{
		HWButton = SYS_RETURNTOMENU;
	}

	/**
	 * Callback for the power button on the Wii.
	 */
	static void WiiPowerPressed()
	{
		HWButton = SYS_POWEROFF_STANDBY;
	}

	/**
 	* Callback for the power button on the Wiimote.
 	* @param[in] chan The Wiimote that pressed the button
 	*/
	static void WiimotePowerPressed(s32 chan)
	{
		HWButton = SYS_POWEROFF_STANDBY;
	}

	void DrawMesh(Mesh* mesh){
		size_t indexCount = mesh->indices.size();
		GX_Begin(GX_TRIANGLES, GX_VTXFMT0, indexCount/3);
		for(size_t i=0;i<indexCount;i+=3){
			GX_Position1x16(mesh->indices[i]);
			GX_Normal1x16(mesh->indices[i+1]);
			GX_TexCoord1x16(mesh->indices[i+2]);
		}
		GX_End();
	}

	void CacheMesh(Mesh* mesh){
		DCInvalidateRange(tempDisplayList,DISPLIST_SIZE);
		DCFlushRange(mesh->verts.data(), 3 * sizeof(float) * mesh->verts.size());
		DCFlushRange(mesh->normals.data(), 3 * sizeof(float) * mesh->normals.size());
		DCFlushRange(mesh->uvs.data(), 2 * sizeof(float) * mesh->uvs.size());
		DCFlushRange(mesh->indices.data(), sizeof(ui16) * mesh->indices.size());
		GX_BeginDispList(tempDisplayList,DISPLIST_SIZE);
		DrawMesh(mesh);
		size_t finalSize = GX_EndDispList();
		if (finalSize > 0 && finalSize < DISPLIST_SIZE){
			mesh->m_gxDispListSize = finalSize;
			DCFlushRange(tempDisplayList, DISPLIST_SIZE);
			mesh->m_gxDispList.resize(mesh->m_gxDispListSize);
			DCInvalidateRange(tempDisplayList,DISPLIST_SIZE);
			memcpy(mesh->m_gxDispList.data(), tempDisplayList, finalSize);
			DCFlushRange(mesh->m_gxDispList.data(), finalSize);
		}
	}

	void ProcessMesh(Mesh* mesh){
		CacheMesh(mesh);
	}

	void DrawCachedMesh(Mesh* mesh){
		GX_SetArray(GX_VA_POS, mesh->verts.data(), 3 * sizeof(float));
		GX_SetArray(GX_VA_NRM, mesh->normals.data(), 3 * sizeof(float));
		GX_SetArray(GX_VA_TEX0, mesh->uvs.data(), 2 * sizeof(float));
		if (mesh->m_gxDispListSize > 0){
			GX_CallDispList(mesh->m_gxDispList.data(), mesh->m_gxDispListSize);
		}
		else
		{
			DrawMesh(mesh);
		}
	}

    void Initialize(){
		fatInitDefault();
		tempDisplayList = memalign(32, DISPLIST_SIZE);
		GXColor	backgroundColor	= {0, 0, 50,	255};
	    void *fifoBuffer = NULL;

	    VIDEO_Init();
	    WPAD_Init();

		SYS_SetResetCallback(WiiResetPressed);
		SYS_SetPowerCallback(WiiPowerPressed);
		WPAD_SetPowerButtonCallback(WiimotePowerPressed);

		settime((uint64_t)0); //So we don't have to start with a huge number.
		startDeltaTime = gettime();

	    screenMode = VIDEO_GetPreferredMode(NULL);

	    frameBuffer	= MEM_K0_TO_K1(SYS_AllocateFramebuffer(screenMode));

	    VIDEO_Configure(screenMode);
	    VIDEO_SetNextFramebuffer(frameBuffer);
	    VIDEO_SetPostRetraceCallback(copy_buffers);
	    VIDEO_SetBlack(false);
	    VIDEO_Flush();

	    fifoBuffer = MEM_K0_TO_K1(memalign(32,FIFO_SIZE));
	    memset(fifoBuffer,	0, FIFO_SIZE);

	    GX_Init(fifoBuffer, FIFO_SIZE);
	    GX_SetCopyClear(backgroundColor, 0x00ffffff);

        GX_SetViewport(0,0,screenMode->fbWidth,screenMode->efbHeight,0,1);
	    GX_SetDispCopyYScale((f32)screenMode->xfbHeight/(f32)screenMode->efbHeight);
	    GX_SetScissor(0,0,screenMode->fbWidth,screenMode->efbHeight);
	    GX_SetDispCopySrc(0,0,screenMode->fbWidth,screenMode->efbHeight);
	    GX_SetDispCopyDst(screenMode->fbWidth,screenMode->xfbHeight);
	    GX_SetCopyFilter(screenMode->aa,screenMode->sample_pattern,
					 GX_TRUE,screenMode->vfilter);
	    GX_SetFieldMode(screenMode->field_rendering,
					((screenMode->viHeight==2*screenMode->xfbHeight)?GX_ENABLE:GX_DISABLE));

	    GX_SetCullMode(GX_CULL_BACK);
	    GX_CopyDisp(frameBuffer,GX_TRUE);
	    GX_SetDispCopyGamma(GX_GM_1_0);

		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

		GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
		GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

		SYS_STDIO_Report(true);
	}

	void TransposeGLMatrix(Mtx gxMatrix, glm::mat4 glmMatrix){
		for (int r = 0; r < 3; r++)
		{
    		for (int c = 0; c < 4; c++)
    		{
        		gxMatrix[r][c] = glmMatrix[c][r];
			}
    	}
	}
	
	void RenderWithCamera(Camera* cam){
		guPerspective(projection, cam->m_fieldOfView, (CONF_GetAspectRatio() == CONF_ASPECT_16_9) ? 16.0F/9.0F : 4.0F/3.0F, cam->m_nearPlane,	cam->m_farPlane);
	    GX_LoadProjectionMtx(projection, GX_PERSPECTIVE);
		glm::mat4 glView = cam->GetViewMatrix();
		TransposeGLMatrix(view, glView);
		GX_SetViewport(0,0,screenMode->fbWidth,screenMode->efbHeight,0,1);
		update_screen(cam->m_scene, view);
	}

    void Update(){
		uint64_t now = gettime();
		Time::DeltaTime = (float)((double)(now - startDeltaTime) / (double)(TB_TIMER_CLOCK * 1000)); // division is to convert from ticks to seconds
		startDeltaTime = now;
		Scene* currentScene = SceneManager::CurrentScene;
		if (currentScene == nullptr) return;
		Camera* currentCam = currentScene->m_activeCamera;
		if (currentCam == nullptr) return;
		RenderWithCamera(currentCam);
		if(HWButton != -1)
		{
			SYS_ResetSystem(HWButton, 0, 0);
		}
    }

	void PrepareMaterial(RenderCommand* cmd, RenderCommand* prev){
		if (prev != nullptr && cmd->m_material == prev->m_material) return;
		GX_LoadTexObj(cmd->m_material->m_texture->m_texObj.get(), GX_TEXMAP0);
		if (prev != nullptr && prev->m_material->m_shader == cmd->m_material->m_shader) return;
		GX_SetNumChans(0);
	    GX_SetNumTexGens(1);
		GX_SetNumTevStages(1);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	    GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
	}

	static std::vector<RenderCommand> currentRenderCommands;
	static Mtx currentViewMatrix;

	void ProcessDebugDraws(){
		Mtx	modelView;
		guMtxIdentity(modelView);
		guMtxTransApply(modelView, modelView, 0.0F,	0.0F, 0.0F);
		guMtxConcat(currentViewMatrix,modelView,modelView);

		GX_LoadPosMtxImm(modelView,	GX_PNMTX0);
		GX_SetCurrentMtx(GX_PNMTX0);

		GX_SetNumChans(1);
		GX_SetChanCtrl(GX_COLOR0A0,GX_DISABLE,GX_SRC_VTX,GX_SRC_VTX,GX_LIGHTNULL,GX_DF_NONE,GX_AF_NONE);
	    GX_SetNumTexGens(0);
		GX_SetNumTevStages(1);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	    GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

		GX_Begin(GX_LINES, GX_VTXFMT1, 2);
		GX_Position3f32(0.0f, 0.0f, 0.0f);
		GX_Color4u8(255, 255, 255, 255);
		GX_Position3f32(0.0f, COLLGRIDSIZE, 0.0f);
		GX_Color4u8(255, 255, 255, 255);
		GX_End();
	}

	void ExecuteRenderCommand(RenderCommand* cmd, RenderCommand* prev){
		Mtx gx;
		TransposeGLMatrix(gx, cmd->m_matrix);
		guMtxConcat(currentViewMatrix,gx,gx);
		GX_LoadPosMtxImm(gx, GX_PNMTX0);
		// call when vtx attributes change
		//GX_ClearVtxDesc();
		// call when dynamic meshes modified
		//GX_InvVtxCache();
		// call when textures modified
		//GX_InvalidateTexAll();
		
		PrepareMaterial(cmd, prev);
		DrawCachedMesh(cmd->m_mesh);
	}

	

	void QueueMeshRenderCommand(glm::mat4 Matrix, Mesh* mesh, Material* material){
		currentRenderCommands.push_back(RenderCommand(Matrix, mesh, material));
	}
	
    void update_screen(	Scene* scene, Mtx	viewMatrix )
    {
		GX_ClearVtxDesc();
		GX_SetVtxDesc(GX_VA_POS, GX_INDEX16);
		GX_SetVtxDesc(GX_VA_NRM, GX_INDEX16);
		GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX16);

		memcpy(currentViewMatrix, viewMatrix, sizeof(Mtx));

		size_t entityCount = scene->m_entities.size();

		for(size_t i=0;i<entityCount;i++){
			Entity* entity = scene->m_entities[i].get();
			if (entity->m_renderable != nullptr){
				entity->m_renderable->QueueRenderCommands();
			}
		}

		size_t renderableCount = currentRenderCommands.size();
		RenderCommand* previousRenderCommand = nullptr;
		for(size_t i=0;i<renderableCount;i++){
			RenderCommand* renderCommand = &currentRenderCommands[i];
			ExecuteRenderCommand(renderCommand, previousRenderCommand);
			previousRenderCommand = renderCommand;
		}

		currentRenderCommands.clear();

		GX_ClearVtxDesc();

		GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
		GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

		ProcessDebugDraws();

	    GX_DrawDone();
	    readyForCopy = GX_TRUE;

	    VIDEO_WaitVSync();
		if (CAPFPS)
			VIDEO_WaitVSync();
	    return;
    }

    static void	copy_buffers(u32 count __attribute__ ((unused)))
    {
	    if (readyForCopy==GX_TRUE) {
		    GX_SetZMode(GX_TRUE, GX_LEQUAL,	GX_TRUE);
		    GX_SetColorUpdate(GX_TRUE);
		    GX_CopyDisp(frameBuffer,GX_TRUE);
		    GX_Flush();
		    readyForCopy = GX_FALSE;
	    }
    }

}