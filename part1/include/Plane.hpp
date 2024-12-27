#pragma once

#include <vector>
#include <array>
#include <glad/glad.h>
#include <glm/vec3.hpp>
#include "Geometry.hpp"

class Plane {
private:
    float m_size;
    std::vector<Geometry::Triangle> m_mesh;
    GLuint m_vbo;
    std::vector<GLfloat> m_vboData;
    std::array<u_int8_t, 3> m_planeColorRGB;
    GLuint m_vertexArrayObject;

    std::vector<GLfloat> GetVertexBufferObjectData();
public:
    Plane(float size, std::array<u_int8_t, 3> planeColorRGB);
    
    void Draw();
};