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
    m_Front(glm::vec3(0.0f, 0.0f, -1.0f)), // Initialized but will be overwritten
    m_MovementSpeed(DEFAULT_SPEED),
    m_MouseSensitivity(DEFAULT_SENSITIVITY),
    m_Fov(DEFAULT_FOV),
    m_AspectRatio(16.0f / 9.0f), // Default aspect ratio
    m_NearPlane(0.1f),
    m_FarPlane(100.0f)
{
    updateCameraVectors();
    setProjection(m_Fov, m_AspectRatio, m_NearPlane, m_FarPlane); // Initialize projection matrix
}

glm::mat4 Camera::getViewMatrix() const
{
    // The view matrix is created using the camera's position, the point it's looking at, and its up vector.
    // The target point is simply the camera's position plus its front vector.
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
    // Calculate the new Front vector from the Yaw and Pitch angles
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_Front = glm::normalize(front);

    // Also re-calculate the Right and Up vector
    // The Right vector is the cross product of the Front vector and the World's Up vector.
    // This gives a vector that is perpendicular to both, pointing to the right.
    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));  
    
    // The camera's local Up vector is the cross product of the Right and Front vectors.
    // This ensures that all three vectors (Front, Right, Up) are orthogonal to each other.
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}