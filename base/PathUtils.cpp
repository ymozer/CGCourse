#pragma once

#include <string>
#include <SDL3/SDL.h>
#include "Log.hpp"


std::string getPrefPath()
{
    char *prefPath = SDL_GetPrefPath("com.adu.muh", "CGCourse");
    if (!prefPath)
    {
        return "";
    }
    std::string pathString(prefPath);
    SDL_free(prefPath);
    return pathString;
}

std::string getPrefPath(const char *fileName)
{
    std::string prefPath = getPrefPath();
    if (prefPath.empty())
    {
        return std::string(fileName);
    }
    return prefPath + fileName;
}
