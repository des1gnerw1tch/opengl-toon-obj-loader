#include "MaterialLoader.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include "Pixel.hpp"
#include "Utils.hpp"
#include <cstdint>

MaterialLoader::MaterialLoader(std::string materialFilePath)    {
    m_materialFilePath = materialFilePath;
    std::ifstream inputFile;
    inputFile.open(materialFilePath);

    if (inputFile.is_open())  {
        std::string line;
        while (std::getline(inputFile,line))  {
            ProcessLineFromMaterialFile(line);
        }
        inputFile.close();
    } else {
        std::cout << "Could not open file " + materialFilePath << std::endl;
    }
}

void MaterialLoader::ProcessLineFromMaterialFile(std::string line)  {
    std::stringstream stream(line);
    std::string tok;
    while(stream >> tok)  {
        if (tok == "map_Kd")    {
            std::string diffuseTextureFileName;
            stream >> diffuseTextureFileName;
            m_diffuseTexture = PPM(Utils::GetDirectoryOfFile(m_materialFilePath) + diffuseTextureFileName);
            //m_diffuseTexture.value().savePPM("./notflipped.ppm"); // TODO: Remove this
            m_diffuseTexture.value().VerticalFlip(); // PPM reads starting at top left, while OpenGL reads bottom left pixel first.
            //m_diffuseTexture.value().savePPM("./flipped.ppm"); // TODO: Remove this
        }
    }
}

std::vector<uint8_t> MaterialLoader::GetDiffuseTexturePixelData() const  {
    if (!m_diffuseTexture.has_value())   {
        // TODO: These should probably throw errors when no diffuse texture is provided
        return std::vector<uint8_t>();
    }
    std::vector<Pixel> pixels = m_diffuseTexture.value().pixelData();
    int maxRange = m_diffuseTexture.value().getMaxRange();
    std::vector<uint8_t> result;
    for (Pixel p : pixels)  {
        result.push_back(p.r);
        result.push_back(p.g);
        result.push_back(p.b);
    }
    return result;
}

int MaterialLoader::GetDiffuseTextureWidth() const {
    if (m_diffuseTexture.has_value())   {
        return m_diffuseTexture.value().getWidth();
    } else {
        // TODO: These should probably throw errors when no diffuse texture is provided
        return -1;
    }
}

int MaterialLoader::GetDiffuseTextureHeight() const {
    if (m_diffuseTexture.has_value())   {
        return m_diffuseTexture.value().getHeight();
    } else {
        return -1;
    }
}

bool MaterialLoader::HasDiffuseTexture() const  {
    return m_diffuseTexture.has_value();
}
