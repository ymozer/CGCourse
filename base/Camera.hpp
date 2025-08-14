#pragma once
#include <glm/glm.hpp>
struct CameraInput
{
    float moveForward = 0.0f; // -1.0 for S, 1.0 for W
    float moveRight = 0.0f;   // -1.0 for A, 1.0 for D
    float moveUp = 0.0f;      // For flying up/down (e.g., Space/Ctrl)
    float mouseDeltaX = 0.0f;
    float mouseDeltaY = 0.0f;
};

// Defines several possible camera movements. Used for abstraction with controller input etc.
enum class Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera
{
public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f,
           float pitch = 0.0f);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::vec3 getPosition() const { return m_Position; }
    glm::vec3 getFront() const { return m_Front; }
    float getYaw() const { return m_Yaw; }
    float getPitch() const { return m_Pitch; }
    float getMovementSpeed() const { return m_MovementSpeed; }
    float getMouseSensitivity() const { return m_MouseSensitivity; }
    float getFov() const { return m_Fov; }
    float getNearPlane() const { return m_NearPlane; }
    float getFarPlane() const { return m_FarPlane; }

    void setProjection(float fovDegrees, float aspectRatio, float nearPlane, float farPlane);
    void setMovementSpeed(float speed) { m_MovementSpeed = speed; }
    void setMouseSensitivity(float sensitivity) { m_MouseSensitivity = sensitivity; }
    void setFov(float fov);
    void setNearPlane(float nearPlane);
    void setFarPlane(float farPlane);

    void update(const CameraInput &input, float deltaTime);

private:
    glm::vec3 m_Position;
    glm::vec3 m_Front;
    glm::vec3 m_Up;
    glm::vec3 m_Right;
    glm::vec3 m_WorldUp;
    glm::mat4 m_ProjectionMatrix;
    float m_Yaw;
    float m_Pitch;
    float m_MovementSpeed;
    float m_MouseSensitivity;
    float m_Fov;
    float m_AspectRatio;
    float m_NearPlane;
    float m_FarPlane;

    void updateCameraVectors();
};