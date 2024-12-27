#pragma once

#include <glm/vec3.hpp>
#include <vector>
#include <glad/glad.h>

class PointLight {
    private:
        glm::vec3 m_position; // World position of light

        glm::vec3 m_ambientColor;
        GLfloat m_ambientStrength;  // Between 0 and 1
        glm::vec3 m_diffuseColor;
        glm::vec3 m_specularColor;

        GLfloat m_constantFallOff;
        GLfloat m_linearFallOff;
        GLfloat m_quadtraticFallOff;
    public:
        PointLight(glm::vec3 position, glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, 
              GLfloat constantFallOff, GLfloat linearFallOff, GLfloat quadtraticFallOff, GLfloat ambientStrength = 0.1f);
        void SetPosition(glm::vec3 position);
        // posx, posy, posz, colorR, colorG, colorB, posx, ...
        std::vector<GLfloat> GetVertexBufferObjectData();
        std::vector<GLuint> GetElementBufferObjectData();
        glm::vec3 GetPosition();

        // Getters for GLSL Uniform

        const float* GetPositionForUniform() const;
        const float* GetAmbientColorForUniform() const;
        const float GetAmbientStrengthForUniform() const;
        const float* GetDiffuseColorForUniform() const;
        const float* GetSpecularColorForUniform() const;
        const float GetConstantFallOffForUniform() const;
        const float GetLinearFallOffForUniform() const;
        const float GetQuadtraticFallOffForUniform() const;
};