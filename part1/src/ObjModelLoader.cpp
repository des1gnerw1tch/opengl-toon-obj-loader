#include "ObjModelLoader.hpp"

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <regex>
#include <array>
#include "Geometry.hpp"
#include <MaterialLoader.hpp>
#include "Utils.hpp"
#include <cstdint>

ObjModelLoader::ObjModelLoader(std::string filePath)    {
    m_objFilePath = filePath;
    std::ifstream inputFile;
    inputFile.open(filePath);

    if (inputFile.is_open())  {
        std::string line;
        while (std::getline(inputFile,line))  {
            ProcessLineFromOBJFile(line);
        }
        inputFile.close();
    } else {
        std::cout << "Could not open file " + filePath << std::endl;
    }
}

void ObjModelLoader::ProcessLineFromOBJFile(std::string line)    {
    std::stringstream stream(line);
    std::string tok;
    while(stream >> tok)  {
        std::string x;
        std::string y;
        if (tok == "v" || tok == "vn") {
            std::string z; 
            stream >> x;
            stream >> y;
            stream >> z;
            glm::vec3 vec(glm::vec3(std::stof(x), std::stof(y), std::stof(z)));
            if (tok =="v")  {
                positions.push_back(vec);
            } else {
                normals.push_back(vec);
            }
        } else if (tok == "vt") {
            stream >> x;
            stream >> y;
            glm::vec2 vec(std::stof(x), std::stof(y));
            textureCoords.push_back(vec);
        } else if (tok == "f")  {
            ProcessFaceLine(stream);
        } else if (tok == "mtllib") {
            std::string materialFileName;
            stream >> materialFileName;
            // Object will look for material in same folder as object
            material = MaterialLoader(Utils::GetDirectoryOfFile(m_objFilePath) + materialFileName);
        }
    }
}

void ObjModelLoader::ProcessFaceLine(std::stringstream &stream)  {
    Geometry::IndexedTriangle t;
    for (int i = 0; i < 3; i++) {
        std::string tok;
        stream >> tok;
        t.vertexIndices[i] = GetVertexIndex(tok);
    }
    triangles.push_back(t);
}

GLuint ObjModelLoader::GetVertexIndex(std::string const &s)   {
    int positionIndex = -1;
    int textureIndex = -1;
    int normalIndex = -1;

    std::size_t firstSlash = s.find("/");
    positionIndex = std::stoi(s.substr(0, firstSlash));
    std::size_t secondSlash = s.substr(firstSlash + 1).find("/") + firstSlash + 1;
    if (secondSlash - firstSlash != 1)  { // If there is a texture index provided
        textureIndex = std::stoi(s.substr(firstSlash + 1, secondSlash));
    }

    normalIndex = std::stoi(s.substr(secondSlash + 1));
    std::array<int, 3> vertexInfo = {positionIndex, textureIndex, normalIndex};
    if (vertexMap.count(vertexInfo) == 0)   {
        return AddUniqueVertexAndReturnIndex(vertexInfo);
    } else {
	    //std::cout << "Duplicate vertex!" << std::endl;
        return vertexMap[vertexInfo];
    }
}

GLuint ObjModelLoader::AddUniqueVertexAndReturnIndex(std::array<int, 3> vertexInfo)  {
    vertexMap[vertexInfo] = vertices.size(); // Add index number of new vertex to hashmap
    Geometry::Vertex newVertex;
    glm::vec3 p = positions[vertexInfo[0] - 1]; // - 1 because .obj files start index starting at 1, there will never be 0//7 or 7//0
    newVertex.x = p.x;
    newVertex.y = p.y;
    newVertex.z = p.z;

    newVertex.r = (float)defaultColorR / 255.0f; // Normalize colors
    newVertex.g = (float)defaultColorG / 255.0f;
    newVertex.b = (float)defaultColorB / 255.0f;
    
    // If a texture index is provided
    if (vertexInfo[1] != -1)    { 
        glm::vec2 t = textureCoords[vertexInfo[1] - 1];
        newVertex.tx = t.x;
        newVertex.ty = t.y;
    } else  {
        newVertex.tx = 0;
        newVertex.ty = 0;
    }

    glm::vec3 n = normals[vertexInfo[2] - 1];
    newVertex.nx = n.x;
    newVertex.ny = n.y;
    newVertex.nz = n.z;

    vertices.push_back(newVertex);
    return vertexMap[vertexInfo];
}

std::vector<GLfloat> ObjModelLoader::GetVertexBufferObjectData() {
    return Geometry::GetGLFloatVector(vertices);
}

std::vector<GLuint> ObjModelLoader::GetElementBufferObjectData()    {
    std::cout << "Number of triangles " << triangles.size() << std::endl;
    return Geometry::GetElementArrayFromTriangles(triangles);
}

bool ObjModelLoader::HasDiffuseTexture()    {
    return material.has_value() && material.value().HasDiffuseTexture();
}

std::vector<uint8_t> ObjModelLoader::GetDiffuseTextureData()  {
    if (!HasDiffuseTexture())  {
        return std::vector<uint8_t>();
    }
    return material.value().GetDiffuseTexturePixelData();
}

int ObjModelLoader::GetDiffuseTextureWidth()    {
    if (!HasDiffuseTexture())  {
        return -1;
    }
    return material.value().GetDiffuseTextureWidth();
}
int ObjModelLoader::GetDiffuseTextureHeight()   {
    if (!HasDiffuseTexture())  {
        return -1;
    }
    return material.value().GetDiffuseTextureHeight();
}
