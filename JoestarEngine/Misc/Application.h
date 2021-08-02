#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "GlobalConfig.h"
#include "../Graphics/Graphics.h"
class Application {
public:
    static Application* appInstance;
    Application() {
        appInstance = this;
    }
    void Run();
    inline Graphics* GetGraphics() {
        return gGraphics_;
    }

private:
    Graphics* gGraphics_;
};

static Graphics* GetGraphics() {
    return Application::appInstance->GetGraphics();
}