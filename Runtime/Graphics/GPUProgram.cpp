#include "GPUProgram.h"
#include "../Misc/GlobalConfig.h"
#include "GPUProgramGL.h"
#include "GPUProgramVulkan.h"
#include "../IO/FileSystem.h"
#include "../Misc/Application.h"
#include "GraphicDefines.h"

namespace Joestar {
    File* GPUProgram::ShaderCodeFile(const char* file) {
        Application* app = Application::GetApplication();
        FileSystem* fs = app->GetSubsystem<FileSystem>();
        String path = fs->GetResourceDir();
        path += "Shaders/";
        path += file;
        File* f = fs->ReadFile(path.CString());
        return f;
    }

    GPUProgram* GPUProgram::CreateProgram() {
        Application* app = Application::GetApplication();
        GFX_API gfxAPI = (GFX_API)(app->GetSubsystem<GlobalConfig>()->GetConfig<int>(CONFIG_GFX_API));

        if (gfxAPI == GFX_API_VULKAN) {
            return new GPUProgramVulkan;
        }
        return new GPUProgramGL;
    }
}