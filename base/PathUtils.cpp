#include <string>
#include <SDL3/SDL.h>
#include "Log.hpp"
#include "PathUtils.hpp"

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

std::string resolveAssetPath(const std::string& relativePath) {
    #ifdef __ANDROID__
        // On Android, paths are relative to the APK's assets root
        return relativePath;
    #else
        // On desktop, prepend the "assets/" directory
        return "assets/" + relativePath;
    #endif
}