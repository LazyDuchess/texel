#include "platform/renderer.h"
#include "game.h"

int	main(void) {
	Renderer::Initialize();
	Game::Initialize();
	
	while (1)
	{
		Game::FrameUpdate();
		Renderer::Update();
	}
	return 0;
}