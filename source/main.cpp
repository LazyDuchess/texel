// adapted from	the	original acube demo	by tkcne.

// enjoy

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <platform/renderer.h>

int	main(void) {
	Renderer::Initialize();
	while (1)
	{
		Renderer::Update();
	}
	return 0;
}