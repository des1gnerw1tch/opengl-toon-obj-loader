#pragma once

#include <string>
#include <vector>
#include <glm/vec3.hpp>
#include <sstream>
#include <unordered_map>
#include <array>
#include <functional> // For std::hash
#include <glad/glad.h>
#include <optional>
#include <MaterialLoader.hpp>
#include <cstdint>

#include "Geometry.hpp"


class ObjModelLoader {
private:
    // Hashes a vertex that is in form 1/4/1, position/texture/normal
    struct VertexHashFunctor { 
        std::size_t operator()(const std::array<int, 3>& a) const {
            std::hash<int> hashIntFunctor;
            return hashIntFunctor(a[0]) * 100 + hashIntFunctor(a[1]) * 10 + hashIntFunctor(a[2]);
        }  
    };
    
    // Raw positions, normals, and texture coordinates from obj file
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> textureCoords;

    // Vertices and triangle data for OpenGL
    std::vector<Geometry::Vertex> vertices;
    std::vector<Geometry::IndexedTriangle> triangles;

    // Memory of vertices already seen in form 7//7 5//5 3//3, returns index of vertices member variable
    std::unordered_map<std::array<int, 3>, int, VertexHashFunctor> vertexMap;

    // Gold Chalice
    u_int8_t defaultColorR = 150;
    u_int8_t defaultColorG = 190;
    u_int8_t defaultColorB = 210;

    // White Astronaut
    //u_int8_t defaultColorR = 255;
    //u_int8_t defaultColorG = 255;
    //u_int8_t defaultColorB = 255;

    // Object will look for material in same folder as object
    std::optional<MaterialLoader> material;
    
    void ProcessLineFromOBJFile(std::string line);

    //i.e "f 14//194 174//195 175//196"
    void ProcessFaceLine(std::stringstream &stream);

    //i.e "14//194"
    GLuint GetVertexIndex(std::string const &s);
    
    // vertexInfo in {positionIndex, textureIndex, normalIndex}
    GLuint AddUniqueVertexAndReturnIndex(std::array<int, 3> vertexInfo);

    std::string m_objFilePath;
    
public:
    ObjModelLoader(std::string filePath);
    
    // Formatted: positionX, positionY, positionZ, colorR, colorB, colorG, normalX, 
    // normalY, normalZ, textureX, textureY, positionX, ...
    std::vector<GLfloat> GetVertexBufferObjectData();

    // Formatted: v1, v2, v3, v1, v2, v3, v1...
    std::vector<GLuint> GetElementBufferObjectData();

    bool HasDiffuseTexture();

    std::vector<uint8_t> GetDiffuseTextureData();

    int GetDiffuseTextureWidth();
    int GetDiffuseTextureHeight();
};