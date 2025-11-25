#define	FIFO_SIZE (256*1024)
#include "platform/renderer.h"
#include "mesh.h"
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

#include "textures_tpl.h"
#include "textures.h"

RenderCommand::RenderCommand(Matrix mtx, Mesh* mesh, Material* material){
	memcpy(m_matrix, mtx, sizeof(Matrix));
	m_mesh = mesh;
	m_material = material;
}

namespace Renderer{
	static Material *redMaterial;
	static Mesh *redMesh;

	static Material *tryceMaterial;
	static Mesh *tryceMesh;

	static Material *belMaterial;
	static Mesh *belMesh;

    static GXRModeObj	*screenMode;
    static void	*frameBuffer;
    static vu8	readyForCopy;

	static Mtx44	projection;
	static Mtx	view;
	static guVector camera =	{2.0F, 2.0F, 0.5F};
	static guVector up =	{0.0F, 1.0F, 0.0F};
	static guVector look	= {0.0F, 1.0F, -1.0F};

    static void update_screen(Mtx viewMatrix);
    static void	copy_buffers(u32 unused);

	static float timer = 0;

	void LoadRedMesh(Mesh* mesh){
		#include "platform/metalHead.inc"
	}

	void LoadTryceMesh(Mesh* mesh){
		#include "platform/blockGuy.inc"
	}

	void LoadBelMesh(Mesh* mesh){
		#include "platform/spaceGirl.inc"
	}

    void Initialize(){
		TPLFile texturesTPL;
		GXTexObj* redTexObj = new GXTexObj();
		GXTexObj* tryceTexObj = new GXTexObj();
		GXTexObj* belTexObj = new GXTexObj();
		TPL_OpenTPLFromMemory(&texturesTPL, (void *)textures_tpl,textures_tpl_size);
		
		TPL_GetTexture(&texturesTPL,metalhead,redTexObj);
		redMaterial = new Material();
		redMaterial->m_shader = SHADER_UNLIT_TEXTURED;
		redMaterial->m_texture = new Texture(redTexObj);
		redMesh = new Mesh();
		LoadRedMesh(redMesh);

		TPL_GetTexture(&texturesTPL,blockguy,tryceTexObj);
		tryceMaterial = new Material();
		tryceMaterial->m_shader = SHADER_UNLIT_TEXTURED;
		tryceMaterial->m_texture = new Texture(tryceTexObj);
		tryceMesh = new Mesh();
		LoadTryceMesh(tryceMesh);

		TPL_GetTexture(&texturesTPL,spacegirl,belTexObj);
		belMaterial = new Material();
		belMaterial->m_shader = SHADER_UNLIT_TEXTURED;
		belMaterial->m_texture = new Texture(belTexObj);
		belMesh = new Mesh();
		LoadBelMesh(belMesh);
	    
		GXColor	backgroundColor	= {0, 0, 0,	255};
	    void *fifoBuffer = NULL;

	    VIDEO_Init();
	    WPAD_Init();

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

        GX_ClearVtxDesc();

		GX_SetVtxDesc(GX_VA_POS, GX_INDEX16);
		GX_SetVtxDesc(GX_VA_NRM, GX_INDEX16);
		GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX16);

	    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	}

    void Update(){
		guPerspective(projection, 45, (CONF_GetAspectRatio() == CONF_ASPECT_16_9) ? 16.0F/9.0F : 4.0F/3.0F, 1.0F,	30.0F);
	    GX_LoadProjectionMtx(projection, GX_PERSPECTIVE);
        guLookAt(view, &camera,	&up, &look);
		GX_SetViewport(0,0,screenMode->fbWidth,screenMode->efbHeight,0,1);
		update_screen(view);

		WPAD_ScanPads();
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) exit(0);
		timer += 0.75f;
    }

	void DoDrawCall(RenderCommand* cmd){
		size_t vertCount = cmd->m_mesh->verts.size();
		GX_SetArray(GX_VA_POS, cmd->m_mesh->verts.data(), 3 * sizeof(float));
		GX_SetArray(GX_VA_NRM, cmd->m_mesh->normals.data(), 3 * sizeof(float));
		GX_SetArray(GX_VA_TEX0, cmd->m_mesh->uvs.data(), 2 * sizeof(float));
		GX_Begin(GX_TRIANGLES, GX_VTXFMT0, vertCount);
		for(size_t i=0;i<vertCount;i++){
			GX_Position1x16(i);
			GX_Normal1x16(i);
			GX_TexCoord1x16(i);
		}
		GX_End();
	}


	void PrepareMaterial(RenderCommand* cmd, RenderCommand* prev){
		if (prev != nullptr && cmd->m_material == prev->m_material) return;
		GX_LoadTexObj(cmd->m_material->m_texture->m_texObj, GX_TEXMAP0);
		if (prev != nullptr && prev->m_material->m_shader == cmd->m_material->m_shader) return;
		GX_SetNumChans(0);
	    GX_SetNumTexGens(1);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	    GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
	}

	void ExecuteRenderCommand(RenderCommand* cmd, RenderCommand* prev){
		GX_LoadPosMtxImm(cmd->m_matrix,	GX_PNMTX0);
		// call when vtx attributes change
		//GX_ClearVtxDesc();
		// call when dynamic meshes modified
		//GX_InvVtxCache();
		// call when textures modified
		//GX_InvalidateTexAll();
		
		PrepareMaterial(cmd, prev);
		DoDrawCall(cmd);
	}
	
    void update_screen(	Mtx	viewMatrix )
    {
	    Mtx	modelView;

	    guMtxIdentity(modelView);
		guVector axis;
		axis.x = 0;
		axis.y = 1;
		axis.z = 0;
		guMtxRotAxisDeg(modelView, &axis, timer);
	    guMtxTransApply(modelView, modelView, 0.0F,0.0F,-1.0F);
	    guMtxConcat(viewMatrix,modelView,modelView);

		RenderCommand redcmd = RenderCommand(modelView, redMesh, redMaterial);
		ExecuteRenderCommand(&redcmd, nullptr);

		guMtxIdentity(modelView);
		guMtxRotAxisDeg(modelView, &axis, timer);
	    guMtxTransApply(modelView, modelView, -2.0F,0.0F,0.0F);
	    guMtxConcat(viewMatrix,modelView,modelView);

		RenderCommand trycecmd = RenderCommand(modelView, tryceMesh, tryceMaterial);
		ExecuteRenderCommand(&trycecmd, &redcmd);

		guMtxIdentity(modelView);
		guMtxRotAxisDeg(modelView, &axis, timer);
	    guMtxTransApply(modelView, modelView, -1.0F,0.0F,-2.0F);
	    guMtxConcat(viewMatrix,modelView,modelView);

		RenderCommand belcmd = RenderCommand(modelView, belMesh, belMaterial);
		ExecuteRenderCommand(&belcmd, &trycecmd);

	    GX_DrawDone();
	    readyForCopy = GX_TRUE;

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