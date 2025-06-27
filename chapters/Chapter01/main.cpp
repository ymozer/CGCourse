#include "Chapter01.hpp" // Include the chapter's class definition
#include "Log.hpp"
#include <memory>
#include <stdexcept>
#include <SDL3/SDL_main.h>

int main(int argc, char *argv[])
{
    try {
        auto app = std::make_unique<Chapter01_Application>();
        app->run(); // run() now only exists for non-Android builds
    } catch (const std::exception& e) {
        LOG_ERROR("Caught exception: {}", e.what());
        return -1;
    }
    return 0;
}