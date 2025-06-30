#pragma once
#include <glm/glm.hpp>

// An intermediate data structure to hold the results of input processing.
// This struct is designed to be written to from multiple threads (e.g., event handlers)
// and read by the main thread once per frame.
// For true thread safety, the floats should either be std::atomic<float> or
// the whole struct should be protected by a mutex when read/written.
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
    // --- Constructor ---
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f,
           float pitch = 0.0f);

    // --- Getters ---
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::vec3 getPosition() const { return m_Position; }
    glm::vec3 getFront() const { return m_Front; }

    // --- Setters / Modifiers ---
    void setProjection(float fovDegrees, float aspectRatio, float nearPlane, float farPlane);

    // This is the main-thread update function.
    // It consumes the processed input and updates the camera's internal state.
    void update(const CameraInput &input, float deltaTime);

private:
    // --- Camera Attributes ---
    glm::vec3 m_Position;
    glm::vec3 m_Front;
    glm::vec3 m_Up;
    glm::vec3 m_Right;
    glm::vec3 m_WorldUp;

    // --- Euler Angles ---
    float m_Yaw;
    float m_Pitch;

    // --- Camera Options ---
    float m_MovementSpeed;
    float m_MouseSensitivity;
    float m_Fov;

    // --- Projection Matrix ---
    glm::mat4 m_ProjectionMatrix;
    float m_AspectRatio;
    float m_NearPlane;
    float m_FarPlane;

    // --- Private Methods ---
    // Recalculates the Front, Right and Up vectors from the updated Euler angles.
    void updateCameraVectors();
};