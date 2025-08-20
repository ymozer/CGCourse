#include "BundledApp.hpp"
#include "Log.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <memory>

int main(int argc, char* argv[])
{
    Logger::getInstance().initialize({});
    try {
        auto app = std::make_unique<BundledApp>();
        app->run();
    } catch (const std::exception& e) {
        LOG_ERROR("A fatal error occurred: {}", e.what());
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR,
            "Application Error",
            e.what(),
            nullptr);
        return -1;
    }
    return 0;
}