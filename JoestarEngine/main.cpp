
#include <iostream>
#include "Misc/GlobalConfig.h"
#include "Misc/Application.h"


int main() {
    uint32_t width = 800, height = 600;
    GetGlobalConfig()->UpdateConfig("WINDOW_WIDTH", width);
    GetGlobalConfig()->UpdateConfig("WINDOW_HEIGHT", height);
    Application app;
    app.Run();

    return 0;
}