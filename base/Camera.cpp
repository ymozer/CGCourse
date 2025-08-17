#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm> // std::clamp

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : m_Position(position),
      m_WorldUp(up),
      m_Yaw(yaw),
      m_Pitch(pitch),
      m_Front(glm::vec3(0.0f, 0.0f, -1.0f)),
      m_Mode(CameraMode::FLY),
      m_MovementSpeed(SPEED),
      m_MouseSensitivity(SENSITIVITY),
      m_Fov(FOV)
{
    updateCameraVectors();
    updateViewMatrix();
}

const glm::mat4 &Camera::getViewMatrix() const
{
    return m_ViewMatrix;
}

const glm::mat4 &Camera::getProjectionMatrix() const
{
    return m_ProjectionMatrix;
}

glm::vec3 Camera::getPosition() const
{
    return m_Position;
}

CameraMode Camera::getMode() const
{
    return m_Mode;
}

float Camera::getMovementSpeed() const
{
    return m_MovementSpeed;
}

float Camera::getMouseSensitivity() const
{
    return m_MouseSensitivity;
}

float Camera::getFov() const
{
    return m_Fov;
}

void Camera::setPosition(const glm::vec3 &position)
{
    m_Position = position;
    updateViewMatrix();
}

void Camera::lookAt(const glm::vec3 &target)
{
    m_Front = glm::normalize(target - m_Position);
    m_Pitch = glm::degrees(asinf(m_Front.y));
    m_Yaw = glm::degrees(atan2f(m_Front.z, m_Front.x));
    updateCameraVectors();
    updateViewMatrix();
}

void Camera::setProjection(float fov, float aspectRatio, float nearPlane, float farPlane)
{
    m_Fov = fov;
    m_AspectRatio = aspectRatio;
    m_NearPlane = nearPlane;
    m_FarPlane = farPlane;
    updateProjectionMatrix();
}

void Camera::setMode(CameraMode mode)
{
    m_Mode = mode;
}

void Camera::setMovementSpeed(float speed)
{
    m_MovementSpeed = speed;
}

void Camera::setMouseSensitivity(float sensitivity)
{
    m_MouseSensitivity = sensitivity;
}

void Camera::setFov(float fov)
{
    m_Fov = fov;
    updateProjectionMatrix();
}

void Camera::update(const CameraInput &input, float deltaTime)
{
    m_Yaw += input.mouseDeltaX * m_MouseSensitivity;
    m_Pitch += input.mouseDeltaY * m_MouseSensitivity;

    // Clamp pitch
    m_Pitch = std::clamp(m_Pitch, -89.0f, 89.0f);

    updateCameraVectors();

    glm::vec3 moveDirection(0.0f);

    if (m_Mode == CameraMode::FPS)
    {
        // In FPS mode, movement is constrained to the XZ plane
        glm::vec3 forward = glm::normalize(glm::vec3(m_Front.x, 0.0f, m_Front.z));
        glm::vec3 right = glm::normalize(glm::vec3(m_Right.x, 0.0f, m_Right.z));
        moveDirection += forward * input.moveForward;
        moveDirection += right * input.moveRight;
        moveDirection += m_WorldUp * input.moveUp;
    }
    else // Fly Mode
    {
        moveDirection += m_Front * input.moveForward;
        moveDirection += m_Right * input.moveRight;
        moveDirection += m_WorldUp * input.moveUp;
    }

    // Normalize the movement vector to ensure consistent speed
    if (glm::length(moveDirection) > 0.0f)
    {
        moveDirection = glm::normalize(moveDirection);
    }

    m_Position += moveDirection * m_MovementSpeed * deltaTime;

    updateViewMatrix();
}

void Camera::updateCameraVectors()
{
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_Front = glm::normalize(front);

    // Also re-calculate the Right and Up vector
    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}

void Camera::updateViewMatrix()
{
    m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

void Camera::updateProjectionMatrix()
{
    m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_NearPlane, m_FarPlane);
}