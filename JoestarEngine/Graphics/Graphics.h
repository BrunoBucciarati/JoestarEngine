#pragma once
#include "Mesh.h"
class Graphics {
public:
	virtual void Init(){}
	virtual void DrawTriangle(){}
	void MainLoop();
	virtual void DrawMesh(Mesh* mesh) {}
};