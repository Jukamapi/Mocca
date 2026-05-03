#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>


// TODO: Implement AssetManager
static std::vector<char> readFile(const std::string& fileName)
{
    std::filesystem::path absolutePath = std::filesystem::absolute(fileName);

    if(!std::filesystem::exists(fileName))
    {
        throw std::runtime_error("file does not exist: " + absolutePath.string());
    }

    // using ate - "at the end" to know size of file
    std::ifstream file(fileName, std::ios::ate | std::ios::binary);

    if(!file.is_open())
    {
        throw std::runtime_error("failed to open file!" + fileName);
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

static std::vector<char> loadShader(const std::string& shaderName)
{
#ifdef SHADER_DIR
    // Cmake path + filename
    std::filesystem::path shaderPath = std::filesystem::path(SHADER_DIR) / shaderName;
    return readFile(shaderPath.string());
#else
    // Fallback
    return readFile("shaders/" + shaderName);
#endif
}
