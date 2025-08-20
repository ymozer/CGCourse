#include "Chapter02.hpp"

#include "Log.hpp"
#include "EventTypes.hpp"
#include "Application.hpp"

#include <memory>
#include <SDL3/SDL_main.h>

int main(int argc, char *argv[])
{
    Logger::getInstance().initialize({});

    try
    {
        auto app = std::make_unique<Chapter02_1_Application>();
        app->run();
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("A fatal error occurred: {}", e.what());
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR,
            "Application Error",
            e.what(),
            NULL);
        return -1;
    }
    return 0;
}
