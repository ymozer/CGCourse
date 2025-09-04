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

std::vector<char> readAssetToBuffer(const std::string& path)
{
    SDL_IOStream* file = SDL_IOFromFile(path.c_str(), "rb");
    if (!file)
    {
        LOG_ERROR("Failed to open asset file with SDL_IOFromFile: {}", path);
        return {}; // Return an empty vector on failure
    }

    Sint64 size = SDL_GetIOSize(file);
    if (size <= 0)
    {
        LOG_ERROR("Asset file is empty or size is invalid: {}", path);
        SDL_CloseIO(file);
        return {};
    }

    std::vector<char> buffer(size);
    size_t read_size = SDL_ReadIO(file, buffer.data(), size);
    SDL_CloseIO(file);

    if (read_size != size)
    {
        LOG_ERROR("Failed to read the full contents of asset: {}", path);
        return {};
    }

    LOG_INFO("Successfully read asset '{}' into memory ({} bytes)", path, size);
    return buffer;
}