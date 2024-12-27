#include "ShadowDirectionalLight.hpp"
#include "glm/gtc/matrix_transform.hpp"

ShadowDirectionalLight::ShadowDirectionalLight(GLenum texUnit, glm::vec3 eyePosition, glm::vec3 target, glm::vec3 upVector,
                                               float nearPlane, float farPlane): 
    m_texUnit(texUnit), m_eyePosition(eyePosition), m_target(target), 
    m_upVector(upVector), m_nearPlane(nearPlane), m_farPlane(farPlane)  {
    glActiveTexture(m_texUnit);

    glGenFramebuffers(1, &m_depthMapFBO);
    glGenTextures(1, &m_depthMapTex);
    glBindTexture(GL_TEXTURE_2D, m_depthMapTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
                m_shadowMapWidth, m_shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Attach depthMap to frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMapTex, 0);

    // No need for color buffer, as we just need depth map for shadows
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);  
    glActiveTexture(GL_TEXTURE0); // Reset active texture unit
}

void ShadowDirectionalLight::ActivateTexUnitAndBindFBO()    {
    glActiveTexture(m_texUnit);
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
}

glm::mat4 ShadowDirectionalLight::GetViewMatrix()   {
    return glm::lookAt(m_eyePosition, 
                m_target, 
                m_upVector);
}

glm::mat4 ShadowDirectionalLight::GetProjectionMatrix()   {
    // TODO: Check this 10.0f number if shadows don't work
    return glm::ortho(-7.0f, 7.0f, -7.0f, 7.0f, m_nearPlane, m_farPlane);
}

glm::mat4 ShadowDirectionalLight::GetWorldToLightSpaceTransform()   {
    return GetProjectionMatrix() * GetViewMatrix();
}