#define	FIFO_SIZE (256*1024)
#include "renderer.h"
#include "../mesh.h"
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

#include "textures_tpl.h"
#include "textures.h"


namespace Renderer{

	static Mesh *testMesh;
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

	static GXTexObj texObj;

	void LoadMesh(Mesh* mesh){
		#include "metalHead.inc"
	}

    void Initialize(){
		TPLFile texturesTPL;
		testMesh = new Mesh();
		LoadMesh(testMesh);
		// make a nice lil colorful triangle
		/*
		testMesh->verts.push_back({0.0f,15.0f,0.0f});
		testMesh->verts.push_back({-15.0f,-15.0f,0.0f});
		testMesh->verts.push_back({15.0f,-15.0f,0.0f});
		testMesh->colors.push_back({1.0f,0.0f,0.0f});
		testMesh->colors.push_back({0.0f,1.0f,0.0f});
		testMesh->colors.push_back({0.0f,0.0f,1.0f});*/

		

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

	    GX_SetCullMode(GX_CULL_NONE);
	    GX_CopyDisp(frameBuffer,GX_TRUE);
	    GX_SetDispCopyGamma(GX_GM_1_0);

        GX_ClearVtxDesc();
		TPL_OpenTPLFromMemory(&texturesTPL, (void *)textures_tpl,textures_tpl_size);
		TPL_GetTexture(&texturesTPL,metalhead,&texObj);
		GX_LoadTexObj(&texObj, GX_TEXMAP0);
	}

    void Update(){
		guPerspective(projection, 45, (CONF_GetAspectRatio() == CONF_ASPECT_16_9) ? 16.0F/9.0F : 4.0F/3.0F, 1.0F,	30.0F);
	    GX_LoadProjectionMtx(projection, GX_PERSPECTIVE);
        guLookAt(view, &camera,	&up, &look);
		GX_SetViewport(0,0,screenMode->fbWidth,screenMode->efbHeight,0,1);
		update_screen(view);

		WPAD_ScanPads();
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) exit(0);
		timer += 0.5f;
    }

	void draw_mesh( Mesh* mesh ){
		GX_ClearVtxDesc();
		GX_InvVtxCache();
		GX_InvalidateTexAll();
		GX_SetNumChans(1);
	    GX_SetNumTexGens(1);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	    GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
		GX_LoadTexObj(&texObj, GX_TEXMAP0);
		size_t vertCount = mesh->verts.size();
		bool hasColors = false;
		bool hasUvs = false;
		GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
		if (mesh->colors.size() == vertCount)
		{
			GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
			GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGB8, 0);
			hasColors = true;
		}
		if (mesh->uvs.size() == vertCount)
		{
			GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
			GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
			hasUvs = true;
		}

		GX_Begin(GX_TRIANGLES, GX_VTXFMT0, vertCount);
		for(size_t i=0;i<vertCount;i++){
			Vec3 vert = mesh->verts[i];
			GX_Position3f32(vert.x, vert.y, vert.z);
			if (hasColors){
				Vec3 color = mesh->colors[i];
				GX_Color3f32(color.x, color.y, color.z);
			}
			if (hasUvs){
				Vec2 uv = mesh->uvs[i];
				GX_TexCoord2f32(uv.x, uv.y);
			}
		}
		GX_End();
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

	    GX_LoadPosMtxImm(modelView,	GX_PNMTX0);
		draw_mesh(testMesh);

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