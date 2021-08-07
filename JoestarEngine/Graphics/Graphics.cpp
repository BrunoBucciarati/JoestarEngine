#include "Graphics.h"
#include "../Base/Scene.h"

static Scene* scene;
void Graphics::MainLoop() {
	if (!scene) {
		scene = new Scene;
	}
	scene->RenderScene();
}