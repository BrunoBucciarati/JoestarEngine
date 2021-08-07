#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "../Base/ClassIDs.h"
#include "../Base/EngineContext.h"
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
            return gContext->GetSubSystem<T>();
        }

    private:
        EngineContext* gContext;
    };
}