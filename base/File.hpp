#pragma once

#include <string>
#include <SDL3/SDL.h>
#include "Log.hpp"


std::string getPrefPath()
{
    char *prefPath = SDL_GetPrefPath("com.adu.muh", "CGCourse");

    if (!prefPath)
    {
        LOG_WARN("SDL_GetPrefPath Error: Could not determine preference directory. {}", SDL_GetError());
        return "";
    }
    SDL_free(prefPath);
    return std::string(prefPath);
}

std::string getPrefPath(const char *fileName)
{
    std::string prefPath = getPrefPath();
    if (prefPath.empty())
    {
        LOG_WARN("Could not get preference path from SDL. Defaulting to local '{}'", fileName);
        return std::string(fileName);
    }
    std::string fullPath(prefPath);
    fullPath += fileName;
    LOG_INFO("Preference path set to: {}", fullPath);
}
