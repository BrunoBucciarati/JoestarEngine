#include "Application.h"
#include "../Graphics/GraphicsGL.h"
#include "../Graphics/GraphicsVulkan.h"

void Application::Run() 
{
	std::string str("OpenGL");
	GetGlobalConfig()->UpdateConfig("GFX_API", str);
	gGraphics_ = new GraphicsGL;
	gGraphics_->Init();
}