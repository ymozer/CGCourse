// Debug.hpp
#pragma once

#if PLATFORM_DESKTOP
#include <glad/gl.h>
#elif PLATFORM_EMSCRIPTEN || PLATFORM_IOS || PLATFORM_ANDROID
#include <glad/gles2.h>
#endif

namespace Base
{
    class Debug
    {
    public:
        static void enableGLDebugOutput();
    private:
#if PLATFORM_DESKTOP
        static void GLAPIENTRY glDebugCallback(
            GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar* message,
            const void* userParam);        
#elif PLATFORM_EMSCRIPTEN || PLATFORM_IOS || PLATFORM_ANDROID       
        static void GLAPIENTRY glDebugCallbackKHR(
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