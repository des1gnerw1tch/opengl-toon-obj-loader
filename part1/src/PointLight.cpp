#include "PointLight.hpp"

PointLight::PointLight(glm::vec3 position, glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, 
                       GLfloat constantFallOff, GLfloat linearFallOff, GLfloat quadtraticFallOff, GLfloat ambientStrength) : 
                       m_position(position),
                       m_ambientColor(ambientColor),
                       m_diffuseColor(diffuseColor),
                       m_specularColor(diffuseColor),
                       m_constantFallOff(constantFallOff),
                       m_linearFallOff(linearFallOff),
                       m_quadtraticFallOff(quadtraticFallOff),
                       m_ambientStrength(ambientStrength){}

void PointLight::SetPosition(glm::vec3 position)  {
    m_position = position;
}

std::vector<GLfloat> PointLight::GetVertexBufferObjectData()    {
    GLfloat r = m_diffuseColor.r;
    GLfloat g = m_diffuseColor.g;
    GLfloat b = m_diffuseColor.b;
    // A cube
    return std::vector<GLfloat> {
        -0.3f, -0.3f, 0.3f, r, g, b, // Front bottom left vertex
        0.3f, -0.3f, 0.3f, r, g, b, // Front bottom right vertex
        0.3f, 0.3f, 0.3f, r, g, b, // Front top right vertex
        -0.3f, 0.3f, 0.3f, r, g, b, // Front top left vertex
        -0.3f, -0.3f, -0.3f, r, g, b, // Back bottom left vertex
        0.3f, -0.3f, -0.3f, r, g, b, // Back bottom right vertex
        0.3f, 0.3f, -0.3f, r, g, b, // Back top right vertex
        -0.3f, 0.3f, -0.3f, r, g, b, // Back top left vertex
    };
}

std::vector<GLuint> PointLight::GetElementBufferObjectData()    {
    return std::vector<GLuint>  {
        // Front face
        0, 1, 2, 
        0, 2, 3,
        // Top
        3, 2, 6,
        3, 6, 7,
        // Back
        4, 6, 5,
        4, 7, 6,
        // Bottom
        0, 5, 1,
        0, 4, 5,
        // Right
        1, 5, 6,
        1, 6, 2,
        // Left
        0, 3, 4,
        4, 3, 7
    };
}

glm::vec3 PointLight::GetPosition() {
    return m_position;
}

const float* PointLight::GetPositionForUniform() const  {
    return &m_position[0];
}
const float* PointLight::GetAmbientColorForUniform() const  {
    return &m_ambientColor[0];
}
const float PointLight::GetAmbientStrengthForUniform() const  {
    return m_ambientStrength;
}
const float* PointLight::GetDiffuseColorForUniform() const  {
    return &m_diffuseColor[0];
}
const float* PointLight::GetSpecularColorForUniform() const {
    return &m_specularColor[0];
}
const float PointLight::GetConstantFallOffForUniform() const    {
    return m_constantFallOff;
}
const float PointLight::GetLinearFallOffForUniform() const  {
    return m_linearFallOff;
}
const float PointLight::GetQuadtraticFallOffForUniform() const  {
    return m_quadtraticFallOff;
}
                    
