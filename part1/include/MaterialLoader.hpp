#pragma once

#include <string>
#include <vector>
#include "PPM.hpp"
#include <optional>
#include <cstdint>

class MaterialLoader    {
private:
    std::string m_materialFilePath;
    std::optional<PPM> m_diffuseTexture;
    void ProcessLineFromMaterialFile(std::string line);
public:
    MaterialLoader(std::string materialFilePath);

    // In RGB format, values between 0-1, starting at bottom left pixel going row by row
    std::vector<uint8_t> GetDiffuseTexturePixelData() const;

    int GetDiffuseTextureWidth() const;
    int GetDiffuseTextureHeight() const;
    bool HasDiffuseTexture() const;
};