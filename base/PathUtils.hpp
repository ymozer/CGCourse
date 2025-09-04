#pragma once

#include <string>


std::string getPrefPath();
std::string getPrefPath(const char *fileName);
std::string resolveAssetPath(const std::string& relativePath);
std::vector<char> readAssetToBuffer(const std::string& path);
