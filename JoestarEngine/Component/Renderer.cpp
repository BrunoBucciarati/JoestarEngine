#include "Renderer.h"
#include "../Misc/Application.h"
#include "../Graphics/Graphics.h"
#include "../Base/GameObject.h"
namespace Joestar {
	void Renderer::Init() {
	}

	void Renderer::Render(Camera cam) {
		if (mesh && mat) {
			Graphics* graphics = GetSubsystem<Graphics>();
			graphics->UpdateMaterial(mat);
			graphics->UpdateBuiltinMatrix(BUILTIN_MATRIX_MODEL, gameObject->GetAfflineTransform());
			graphics->DrawMesh(mesh, mat);
		}
	}
}