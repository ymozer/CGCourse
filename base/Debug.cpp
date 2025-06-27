#include "Debug.hpp"

// We need access to the application's GLAD context and logger.
#include "Application.hpp"
#include "Log.hpp"

// Include the correct GLAD header for direct function calls
#if PLATFORM_DESKTOP
    #include <glad/gl.h>
#endif


namespace Base
{

#if PLATFORM_DESKTOP
    void Debug::enableGLDebugOutput()
    {
        // CHANGED: Removed the glad_context reference.
        // We will call GL functions directly.

        // A debug context must have been requested and created.
        // This is done via SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
        GLint flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags); // CHANGED
        if (!(flags & GL_CONTEXT_FLAG_DEBUG_BIT))
        {
            LOG_WARN("OpenGL Debug Context not available. Debug output disabled.");
            LOG_WARN("Hint: Did you call SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG)?");
            return;
        }

        // Check if the modern debug message callback is available.
        // gladDebugMessageCallback is the function pointer loaded by GLAD.
        if (!glad_glDebugMessageCallback) // CHANGED
        {
            LOG_WARN("glDebugMessageCallback not available. OpenGL debug output disabled.");
            return;
        }

        LOG_INFO("Enabling OpenGL debug output.");
        glEnable(GL_DEBUG_OUTPUT); // CHANGED
        // Make the output synchronous. This means the callback will be called
        // from within the GL function that triggered it, which is useful for debugging.
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // CHANGED
        glDebugMessageCallback(glDebugCallback, nullptr); // CHANGED

        // Configure which messages to receive.
        // We disable notifications as they can be verbose and are often not errors.
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE); // CHANGED
    }

    void GLAPIENTRY Debug::glDebugCallback(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        const void* userParam)
    {
        // This function was already correct as it didn't use glad_context.
        // No changes are needed here.
        
        // Ignore some non-significant error codes
        if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
            return;

        const char* sourceStr;
        switch (source)
        {
            case GL_DEBUG_SOURCE_API: sourceStr = "API"; break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceStr = "Window System"; break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceStr = "Shader Compiler"; break;
            case GL_DEBUG_SOURCE_THIRD_PARTY: sourceStr = "Third Party"; break;
            case GL_DEBUG_SOURCE_APPLICATION: sourceStr = "Application"; break;
            case GL_DEBUG_SOURCE_OTHER: sourceStr = "Other"; break;
            default: sourceStr = "Unknown"; break;
        }

        const char* typeStr;
        switch (type)
        {
            case GL_DEBUG_TYPE_ERROR: typeStr = "Error"; break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeStr = "Deprecated Behavior"; break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: typeStr = "Undefined Behavior"; break;
            case GL_DEBUG_TYPE_PORTABILITY: typeStr = "Portability"; break;
            case GL_DEBUG_TYPE_PERFORMANCE: typeStr = "Performance"; break;
            case GL_DEBUG_TYPE_MARKER: typeStr = "Marker"; break;
            case GL_DEBUG_TYPE_PUSH_GROUP: typeStr = "Push Group"; break;
            case GL_DEBUG_TYPE_POP_GROUP: typeStr = "Pop Group"; break;
            case GL_DEBUG_TYPE_OTHER: typeStr = "Other"; break;
            default: typeStr = "Unknown"; break;
        }

        // Log messages based on their severity
        switch (severity)
        {
            case GL_DEBUG_SEVERITY_HIGH:
                LOG_ERROR("[GL-DBG] [{}|{}] ID:{}: {}", sourceStr, typeStr, id, message);
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                LOG_WARN("[GL-DBG] [{}|{}] ID:{}: {}", sourceStr, typeStr, id, message);
                break;
            case GL_DEBUG_SEVERITY_LOW:
                LOG_INFO("[GL-DBG] [{}|{}] ID:{}: {}", sourceStr, typeStr, id, message);
                break;
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                // These are disabled by DebugMessageControl, but we handle them just in case.
                LOG_TRACE("[GL-DBG] [{}|{}] ID:{}: {}", sourceStr, typeStr, id, message);
                break;
            default:
                LOG_TRACE("[GL-DBG] [{}|{}] ID:{}: {}", sourceStr, typeStr, id, message);
                break;
        }
    }

#else
    // Provide a stub for non-desktop platforms so the code can link.
    void Debug::enableGLDebugOutput()
    {
        // OpenGL debug output is not implemented for this platform.
    }
#endif // PLATFORM_DESKTOP

} // namespace Base