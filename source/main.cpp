#include "platform/renderer.h"
#include "app.h"
#include <cstdio>

int main(int argc, char** argv) {
	Renderer::Initialize();
	App::Initialize();
	while (1)
	{
		App::FrameUpdate();
		Renderer::Update();
	}
	return 0;
}