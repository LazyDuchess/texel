#include "platform/renderer.h"
#include "app.h"

int	main(void) {
	Renderer::Initialize();
	App::Initialize();
	
	while (1)
	{
		App::FrameUpdate();
		Renderer::Update();
	}
	return 0;
}