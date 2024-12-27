#include "Camera.hpp"

#include "glm/gtx/transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/string_cast.hpp>

#include <iostream>

void Camera::MouseLook(int mouseXRel, int mouseYRel){
    // Record our new position as a vector
    glm::vec2 newMousePosition(mouseXRel, mouseYRel);
    // Little hack for our 'mouse look function'
    // We need this so that we can move our camera
    // for the first time.
    static bool firstLook=true;
    if(true == firstLook){
        firstLook=false;
    }

    // Rotate about the upVector
    glm::mat4 rotation = glm::rotate(glm::radians((float) -mouseXRel), m_upVector);
    m_viewDirection = glm::vec3(rotation * glm::vec4(m_viewDirection.x, m_viewDirection.y, m_viewDirection.z, 0)); // 0 because this is a direction
}

void Camera::MoveForward(float speed){
    // Can only should move on the XY plane
    glm::mat4 translation = glm::translate(glm::mat4(), speed * glm::vec3(m_viewDirection.x, 0, m_viewDirection.z));
    m_eyePosition = glm::vec3(translation * glm::vec4(m_eyePosition.x, m_eyePosition.y, m_eyePosition.z, 1)); // 1 because this is a point
    //m_eyePosition = (m_viewDirection * speed) + m_eyePosition;
}

void Camera::MoveBackward(float speed){
    this->MoveForward(-speed);
}

void Camera::MoveLeft(float speed){
    glm::mat4 rotation = glm::rotate(glm::radians(90.0f), m_upVector);
    glm::vec3 rotatedViewDirection = glm::vec3(rotation * glm::vec4(m_viewDirection.x, 0, m_viewDirection.z, 0));
    glm::mat4 translation = glm::translate(glm::mat4(), speed * glm::vec3(rotatedViewDirection.x, 0, rotatedViewDirection.z));
    m_eyePosition = glm::vec3(translation * glm::vec4(m_eyePosition.x, m_eyePosition.y, m_eyePosition.z, 1)); // 1 because this is a point
}

void Camera::MoveRight(float speed){
    this->MoveLeft(-speed);
}

void Camera::MoveUp(float speed){
    m_eyePosition.y += speed;
}

void Camera::MoveDown(float speed){
    m_eyePosition.y -= speed;
}

// Set the position for the camera
void Camera::SetCameraEyePosition(float x, float y, float z){
    m_eyePosition.x = x;
    m_eyePosition.y = y;
    m_eyePosition.z = z;
}

float Camera::GetEyeXPosition(){
    return m_eyePosition.x;
}

float Camera::GetEyeYPosition(){
    return m_eyePosition.y;
}

float Camera::GetEyeZPosition(){
    return m_eyePosition.z;
}

float Camera::GetViewXDirection(){
    return m_viewDirection.x;
}

float Camera::GetViewYDirection(){
    return m_viewDirection.y;
}

float Camera::GetViewZDirection(){
    return m_viewDirection.z;
}


Camera::Camera(){
    std::cout << "Camera.cpp: (Constructor) Created a Camera!\n";
	// Position us at in front of origin by 3f. 
    m_eyePosition = glm::vec3(0.0f,0.0f, 3.0f);
	// Looking down along the z-axis initially.
	// Remember, this is negative because we are looking 'into' the scene.
    m_viewDirection = glm::vec3(0.0f,0.0f, -1.0f);
	// For now--our upVector always points up along the y-axis
    m_upVector = glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::mat4 Camera::GetViewMatrix() const{
    // Think about the second argument and why that is
    // setup as it is.
    return glm::lookAt( m_eyePosition,
                        m_eyePosition + m_viewDirection,
                        m_upVector);
}
