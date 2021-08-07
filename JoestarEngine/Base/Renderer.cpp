#include "Renderer.h"
#include "../Misc/Application.h"
void Renderer::Render(Camera cam) {
	if (mat_ && mesh_) {
		//mat_->program->SetMat4("projection", cam.GetProjectionMatrix());
		//mat_->program->SetMat4("view", cam.GetProjectionMatrix());
		//mat_->program->SetMat4("model", glm::mat4(1.0f));

		//mat_->program->Use();
		//GetGraphics()->DrawMesh(mesh_);
		//gGraphics->DrawMesh(mesh_);
	}
}