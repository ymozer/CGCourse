#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraMode
{
    FLY,
    FPS
};

struct CameraInput
{
    float moveForward = 0.0f;
    float moveRight = 0.0f;
    float moveUp = 0.0f;
    float mouseDeltaX = 0.0f;
    float mouseDeltaY = 0.0f;
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
const float SENSITIVITY = 0.1f;
const float FOV = 45.0f;

class Camera
{
public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = YAW,
           float pitch = PITCH);

    const glm::mat4 &getViewMatrix() const;
    const glm::mat4 &getProjectionMatrix() const;
    glm::vec3 getPosition() const;
    CameraMode getMode() const;
    float getMovementSpeed() const;
    float getMouseSensitivity() const;
    float getFov() const;

    void setPosition(const glm::vec3 &position);
    void lookAt(const glm::vec3 &target);
    void setProjection(float fov, float aspectRatio, float nearPlane, float farPlane);
    void setMode(CameraMode mode);
    void setMovementSpeed(float speed);
    void setMouseSensitivity(float sensitivity);
    void setFov(float fov);

    void update(const CameraInput &input, float deltaTime);

private:
    void updateCameraVectors();
    void updateViewMatrix();
    void updateProjectionMatrix();

    glm::vec3 m_Position;
    glm::vec3 m_Front;
    glm::vec3 m_Up;
    glm::vec3 m_Right;
    glm::vec3 m_WorldUp;

    float m_Yaw;
    float m_Pitch;

    CameraMode m_Mode;
    float m_MovementSpeed;
    float m_MouseSensitivity;

    float m_Fov;
    float m_AspectRatio;
    float m_NearPlane;
    float m_FarPlane;

    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ProjectionMatrix;
};