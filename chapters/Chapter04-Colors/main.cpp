#include "Chapter04.hpp"

#include "Application.hpp"
#include "Log.hpp"
#include "Shader.hpp"
#include "EventTypes.hpp"

#include <memory>

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>


int main(int argc, char *argv[])
{
    Logger::getInstance().initialize({});

    try
    {
        auto app = std::make_unique<Chapter04_Application>(
            "Chapter 04 - Colors",
            1280,  // Width
            720    // Height
        );
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