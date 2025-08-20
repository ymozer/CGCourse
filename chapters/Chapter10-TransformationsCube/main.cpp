#include "Chapter10.hpp"
#include "Log.hpp"

#include <memory>
#include <SDL3/SDL_main.h>

int main(int argc, char* argv[])
{
    Logger::getInstance().initialize({});
    try {
        auto app = std::make_unique<Chapter10_Application>();
        app->run();
    } catch (const std::exception& e) {
        LOG_ERROR("Fatal error: {}", e.what());
        return -1;
    }
    return 0;
}