#pragma once
#include "../Graphics/Mesh.h"
#include "../Graphics/Material.h"
#include "Camera.h"
class Renderer {
public:
	Mesh* mesh_;
	Material* mat_;
	void Render(Camera cam);
};