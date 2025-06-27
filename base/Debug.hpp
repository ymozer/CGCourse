// Debug.hpp
#pragma once

#if PLATFORM_DESKTOP
    #include <glad/gl.h>
#endif

namespace Base
{
    /**
     * @class Debug
     * @brief A utility class for OpenGL debugging features.
     */
    class Debug
    {
    public:
        /**
         * @brief Enables OpenGL debug output.
         * 
         * This function sets up a callback to receive debug messages from the OpenGL driver.
         * It requires a debug context to be created, which can be requested via
         * `glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE)`.
         * This functionality is only available on PLATFORM_DESKTOP.
         */
        static void enableGLDebugOutput();

    private:
#if PLATFORM_DESKTOP
        // The callback function that will be invoked by OpenGL with debug messages.
        static void GLAPIENTRY glDebugCallback(
            GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar* message,
            const void* userParam);
#endif
    };

} // namespace Base