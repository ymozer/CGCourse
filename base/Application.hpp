#pragma once

#include <string>
#include <memory>
#if PLATFORM_DESKTOP
#include <glad/gl.h>
#elif PLATFORM_ANDROID || PLATFORM_IOS || PLATFORM_EMSCRIPTEN
#include <glad/gles2.h>
#endif

#include <imgui.h>
#include <SDL3/SDL.h>

namespace Base
{

    class Application
    {
    public:

        struct AppContext
        {
            SDL_Window *window;
            SDL_GLContext glcontext;
            bool app_quit = false;
        };

        Application(std::string title, int width = 1280, int height = 720);
        virtual ~Application();

        void run();

        // Non-copyable
        Application(const Application &) = delete;
        Application &operator=(const Application &) = delete;

    protected:
        // Methods for derived classes (chapters) to implement
        virtual void setup() = 0;
        virtual void shutdown() = 0;
        virtual void update(float deltaTime) = 0;
        virtual void render() = 0;
        virtual void renderChapterUI() {}

        SDL_Window *getWindow() const { return appContext.window; }
        int getWidth() const { return m_Width; }
        int getHeight() const { return m_Height; }

    private:
        void init();
        void cleanup();

        void handleEvents();
        void updateRenderingAndWorkAreas();

        void initImGui();
        void beginImGuiFrame();
        void endImGuiFrame();
        void renderUI();

        void mainLoopIteration();

        void createFramebuffer();
        void resizeFramebuffer(int width, int height);
        void cleanupFramebuffer();

        std::string m_ImGuiIniPath; 
        AppContext appContext;
        
        std::string m_Title;
        bool m_Running = true;
        bool m_isMinimized = false;
        int m_Width;
        int m_Height;
        SDL_Rect m_WorkArea;
        SDL_Rect m_RenderArea;


        float m_StyleScale = 1.0f;
        float m_FontScale = 1.0f;
        ImGuiStyle m_BaseStyle;

        bool m_VSync = true;
        int m_FpsLimit = 60;
        uint64_t m_LastFrameTimeCounter = 0;
        uint64_t m_PerfCounterFreq = 1;
        GLuint m_GpuTimeQueries[2] = {0};
        float m_CpuTime_ms = 0.0f;
        float m_GpuTime_ms = 0.0f;
        uint64_t m_FrameCount = 0;

        GLuint m_FboID = 0;
        GLuint m_ColorAttachmentID = 0;
        GLuint m_DepthAttachmentID = 0;
        int m_ViewportWidth = 0;
        int m_ViewportHeight = 0;

        // For the Emscripten main loop
        static Application *s_Instance;
#if PLATFORM_EMSCRIPTEN
        static void emscriptenMainLoop(void *arg);
#endif
    };

} // namespace Base