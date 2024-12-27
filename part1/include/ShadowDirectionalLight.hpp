#pragma once
#include <glad/glad.h>
#include "glm/glm.hpp"

/*
This is a light that does not provide any light. It just provides a shadow. This way, lights and shadows are decoupled
in case we need a light that doesn't cast shadows.
*/ 

class ShadowDirectionalLight    {
private:
    GLenum m_texUnit; // Texture unit to bind depth (shadow) map to
    unsigned int m_depthMapFBO;
    unsigned int m_depthMapTex;
    const unsigned int m_shadowMapWidth = 1024;
    const unsigned int m_shadowMapHeight = 1024;
    float m_nearPlane;
    float m_farPlane;
    glm::vec3 m_eyePosition;
    glm::vec3 m_target;
    glm::vec3 m_upVector;
public:
    ShadowDirectionalLight(GLenum texUnit, glm::vec3 eyePosition, glm::vec3 target, glm::vec3 upVector, float nearPlane, float farPlane);
    inline GLenum GetTexUnit()  {return m_texUnit;}
    inline unsigned int GetShadowMapWidth() { return m_shadowMapWidth;}
    inline unsigned int GetShadowMapHeight() { return m_shadowMapHeight;}
    void ActivateTexUnitAndBindFBO();
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();
    glm::mat4 GetWorldToLightSpaceTransform();
    inline void SetEyePosition(glm::vec3 eyePosition) { m_eyePosition = eyePosition;}
};