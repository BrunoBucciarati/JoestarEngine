#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "../Core/ClassIDs.h"
#include "../Core/EngineContext.h"
#include "../Graphics/Graphics.h"
namespace Joestar {
    class Application {
    public:
        void Run();
        void Start();
        void Update();
        inline EngineContext* GetEngineContext() {
            return gContext;
        }
        static Application* GetApplication();
        template<class T>
        inline T* GetSubSystem() {
            return gContext->GetSubsystem<T>();
        }

    private:
        EngineContext* gContext;
    };
}