#include "Camera.hpp"
#include <cmath>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

const float DEFAULT_YAW         = -90.0f;
const float DEFAULT_PITCH       = 0.0f;
const float DEFAULT_SPEED       = 5.0f;
const float DEFAULT_SENSITIVITY = 0.1f;
const float DEFAULT_FOV         = 45.0f;

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :
    m_Position(position),
    m_WorldUp(up),
    m_Yaw(yaw),
    m_Pitch(pitch),
    m_Front(glm::vec3(0.0f, 0.0f, -1.0f)),
    m_MovementSpeed(5.0f),
    m_MouseSensitivity(0.1f),
    m_Fov(45.0f),
    m_AspectRatio(16.0f / 9.0f),
    m_NearPlane(0.1f),
    m_FarPlane(100.0f)
{
    updateCameraVectors();
    setProjection(m_Fov, m_AspectRatio, m_NearPlane, m_FarPlane);
}

void Camera::setFov(float fov)
{
    m_Fov = fov;
    setProjection(m_Fov, m_AspectRatio, m_NearPlane, m_FarPlane);
}

void Camera::setNearPlane(float nearPlane)
{
    m_NearPlane = nearPlane;
    setProjection(m_Fov, m_AspectRatio, m_NearPlane, m_FarPlane);
}

void Camera::setFarPlane(float farPlane)
{
    m_FarPlane = farPlane;
    setProjection(m_Fov, m_AspectRatio, m_NearPlane, m_FarPlane);
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

glm::mat4 Camera::getProjectionMatrix() const
{
    return m_ProjectionMatrix;
}

void Camera::setProjection(float fovDegrees, float aspectRatio, float nearPlane, float farPlane)
{
    m_Fov = fovDegrees;
    m_AspectRatio = aspectRatio;
    m_NearPlane = nearPlane;
    m_FarPlane = farPlane;
    m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_NearPlane, m_FarPlane);
}

void Camera::update(const CameraInput& input, float deltaTime)
{
    // --- 1. Process Rotation (Mouse Look) ---
    float xoffset = input.mouseDeltaX * m_MouseSensitivity;
    float yoffset = input.mouseDeltaY * m_MouseSensitivity;

    m_Yaw   += xoffset;
    m_Pitch += yoffset;

    // Constrain the pitch to avoid flipping the camera
    // TODO: make it configurable
    if (m_Pitch > 89.0f)
        m_Pitch = 89.0f;
    if (m_Pitch < -89.0f)
        m_Pitch = -89.0f;

    // After updating yaw and pitch, we must recalculate the direction vectors
    updateCameraVectors();

    // --- 2. Process Movement (Keyboard) ---
    float velocity = m_MovementSpeed * deltaTime;
    
    // Normalize the input vectors to ensure consistent speed regardless of direction
    glm::vec3 moveDir(input.moveRight, input.moveUp, -input.moveForward); // Z is negative because forward is -Z
    if (glm::length(moveDir) > 0.0f)
    {
        moveDir = glm::normalize(moveDir);
    }
    
    m_Position += m_Front * moveDir.z * velocity;
    m_Position += m_Right * moveDir.x * velocity;
    m_Position += m_Up    * moveDir.y * velocity;
}


void Camera::updateCameraVectors()
{
    glm::vec3 front;

    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    
    m_Front = glm::normalize(front);
    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));  
    m_Up    = glm::normalize(glm::cross(m_Right, m_Front));
}