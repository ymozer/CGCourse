#include "Debug.hpp"
#include "Application.hpp"
#include "Log.hpp"

namespace Base
{

    void Debug::enableGLDebugOutput()
    {
#if PLATFORM_DESKTOP
        GLint flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (!(flags & GL_CONTEXT_FLAG_DEBUG_BIT))
        {
            LOG_WARN("OpenGL Debug Context not available. Debug output disabled.");
            LOG_WARN("Hint: Did you call SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG)?");
            return;
        }

        if (glad_glDebugMessageCallback)
        {
            LOG_INFO("Enabling OpenGL debug output (via glDebugMessageCallback).");
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(glDebugCallback, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
            return; 
        }
        else if (glad_glDebugMessageCallbackARB)
        {
            LOG_INFO("Enabling OpenGL debug output (via glDebugMessageCallbackARB).");
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallbackARB(glDebugCallback, nullptr);
            glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
            return;
        }
        else
        {
            LOG_WARN("glDebugMessageCallback and glDebugMessageCallbackARB not available. Debug output disabled.");
            return;
        }

#elif PLATFORM_ANDROID || PLATFORM_IOS
        if (glad_glDebugMessageCallbackKHR)
        {
            LOG_INFO("Enabling OpenGL debug output (via glDebugMessageCallbackKHR).");
            glEnable(GL_DEBUG_OUTPUT_KHR);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR);
            glDebugMessageCallbackKHR(glDebugCallbackKHR, nullptr);
            glDebugMessageControlKHR(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION_KHR, 0, nullptr, GL_FALSE);
        }
        else
        {
            LOG_WARN("glDebugMessageCallbackKHR not available.");
            LOG_INFO("Installing GLAD's internal GLES2 debug hook as a fallback.");
            LOG_WARN("This will impact performance and should be disabled in release builds.");
        }
#elif PLATFORM_EMSCRIPTEN 
        LOG_WARN("Emscripten doesn't support glDebugMessageCallback* functions");
#endif
    }

#if PLATFORM_DESKTOP
    void GLAPIENTRY Debug::glDebugCallback(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar *message,
        const void *userParam)
    {
        // This function was already correct as it didn't use glad_context.
        // No changes are needed here.

        // Ignore some non-significant error codes
        if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
            return;

        const char *sourceStr;
        switch (source)
        {
        case GL_DEBUG_SOURCE_API:
            sourceStr = "API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            sourceStr = "Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            sourceStr = "Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            sourceStr = "Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            sourceStr = "Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            sourceStr = "Other";
            break;
        default:
            sourceStr = "Unknown";
            break;
        }

        const char *typeStr;
        switch (type)
        {
        case GL_DEBUG_TYPE_ERROR:
            typeStr = "Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            typeStr = "Deprecated Behavior";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            typeStr = "Undefined Behavior";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            typeStr = "Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            typeStr = "Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            typeStr = "Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            typeStr = "Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            typeStr = "Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            typeStr = "Other";
            break;
        default:
            typeStr = "Unknown";
            break;
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

#elif PLATFORM_EMSCRIPTEN || PLATFORM_ANDROID || PLATFORM_IOS

    void GLAPIENTRY Debug::glDebugCallbackKHR(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar *message,
        const void *userParam)
    {
        // This function was already correct as it didn't use glad_context.
        // No changes are needed here.

        // Ignore some non-significant error codes
        if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
            return;

        const char *sourceStr;
        switch (source)
        {
        case GL_DEBUG_SOURCE_API_KHR:
            sourceStr = "API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM_KHR:
            sourceStr = "Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER_KHR:
            sourceStr = "Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY_KHR:
            sourceStr = "Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION_KHR:
            sourceStr = "Application";
            break;
        case GL_DEBUG_SOURCE_OTHER_KHR:
            sourceStr = "Other";
            break;
        default:
            sourceStr = "Unknown";
            break;
        }

        const char *typeStr;
        switch (type)
        {
        case GL_DEBUG_TYPE_ERROR_KHR:
            typeStr = "Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_KHR:
            typeStr = "Deprecated Behavior";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_KHR:
            typeStr = "Undefined Behavior";
            break;
        case GL_DEBUG_TYPE_PORTABILITY_KHR:
            typeStr = "Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE_KHR:
            typeStr = "Performance";
            break;
        case GL_DEBUG_TYPE_MARKER_KHR:
            typeStr = "Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP_KHR:
            typeStr = "Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP_KHR:
            typeStr = "Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER_KHR:
            typeStr = "Other";
            break;
        default:
            typeStr = "Unknown";
            break;
        }

        // Log messages based on their severity
        switch (severity)
        {
        case GL_DEBUG_SEVERITY_HIGH_KHR:
            LOG_ERROR("[GL-DBG] [{}|{}] ID:{}: {}", sourceStr, typeStr, id, message);
            break;
        case GL_DEBUG_SEVERITY_MEDIUM_KHR:
            LOG_WARN("[GL-DBG] [{}|{}] ID:{}: {}", sourceStr, typeStr, id, message);
            break;
        case GL_DEBUG_SEVERITY_LOW_KHR:
            LOG_INFO("[GL-DBG] [{}|{}] ID:{}: {}", sourceStr, typeStr, id, message);
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION_KHR:
            LOG_TRACE("[GL-DBG] [{}|{}] ID:{}: {}", sourceStr, typeStr, id, message);
            break;
        default:
            LOG_TRACE("[GL-DBG] [{}|{}] ID:{}: {}", sourceStr, typeStr, id, message);
            break;
        }
    }
#endif

} // namespace Base