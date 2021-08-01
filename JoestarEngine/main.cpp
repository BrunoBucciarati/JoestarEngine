
#include <iostream>
#include "GlobalConfig.h"
#include "VulkanApplication.h"


int main() {
    uint32_t width = 800, height = 600;
    GetGlobalConfig()->UpdateConfig("WINDOW_WIDTH", width);
    GetGlobalConfig()->UpdateConfig("WINDOW_HEIGHT", height);
    VulkanApplication app;
    app.Run();

    return 0;
}