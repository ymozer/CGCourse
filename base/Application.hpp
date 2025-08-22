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

#include "Camera.hpp"
#include "EventBus.hpp"

namespace Base
{
    class Application
    {
    public:
        struct AppContext
        {
            SDL_Window *window;
            SDL_GLContext glcontext;
        };

        Application(std::string title="", int width = 1280, int height = 720, int numOfThreads = 4);
        Application(const Application &) = delete;
        Application &operator=(const Application &) = delete;
        virtual ~Application();

        static Application& getInstance() { return *s_Instance; }

        void run();
        float getViewportAspectRatio() const;

    protected:
        virtual void setup() = 0;
        virtual void shutdown() = 0;
        virtual void update(float deltaTime) = 0;
        virtual void handleInput(float deltaTime) = 0;
        virtual void render() = 0;
        virtual void renderChapterUI() {}

        SDL_Window *getWindow() const { return appContext.window; }
        int getWidth() const { return m_Width; }
        int getHeight() const { return m_Height; }
        int getViewportWidth() const { return m_ViewportWidth; }
        int getViewportHeight() const { return m_ViewportHeight; }

        virtual Camera* getActiveCamera() { return nullptr; }
        ParallelEventBus &getEventBus() { return m_EventBus; }
        const ParallelEventBus &getEventBus() const { return m_EventBus; }
        bool isViewportHovered() const { return m_ViewportHovered; }

        template <typename EventType>
        void subscribeToEvent(SubscriptionHandle &handle, std::function<void(EventType &)> handler)
        {
            getEventBus().unsubscribe(handle);
            handle = getEventBus().subscribe(std::move(handler));
        }

        void subscribeToKeys(std::function<void(Base::KeyPressedEvent &)> handler)
        {
            subscribeToEvent(m_KeySub, std::move(handler));
        }

        void subscribeToMouse(std::function<void(Base::MouseMovedEvent &)> handler)
        {
            subscribeToEvent(m_MouseSub, std::move(handler));
        }

        void subscribeToKeyReleases(std::function<void(Base::KeyReleasedEvent &)> handler)
        {
            subscribeToEvent(m_KeyReleaseSub, std::move(handler));
        }

        void subscribeToMouseButtons(std::function<void(Base::MouseButtonPressedEvent &)> handler)
        {
            subscribeToEvent(m_MouseButtonSub, std::move(handler));
        }

    private:
        void init();
        void cleanup();

        void handleEvents();
        void updateRenderingAndWorkAreas();
        void updateStyleAndFonts(float scale);

        void initImGui();
        void beginImGuiFrame();
        void endImGuiFrame();
        void renderUI();

        void mainLoopIteration();

        void createFramebuffer();
        void resizeFramebuffer(int width, int height);
        void cleanupFramebuffer();

        AppContext appContext;
        ParallelEventBus m_EventBus;

        SubscriptionHandle m_KeySub;
        SubscriptionHandle m_MouseSub;
        SubscriptionHandle m_AppQuitSubscription;
        SubscriptionHandle m_WindowCloseSubscription;
        SubscriptionHandle m_KeyReleaseSub;
        SubscriptionHandle m_MouseButtonSub;

        std::string m_Title;
        bool m_Running = true;
        bool m_imGuiEnabled = false;
        bool m_isMinimized = false;
        bool m_ViewportHovered = false;

        int m_Width = 0;
        int m_Height = 0;
        SDL_Rect m_WorkArea{};
        SDL_Rect m_RenderArea{};

        float m_RenderScale = 1.0f;
        float m_StyleScale = 1.0f;
                
        ImGuiStyle m_BaseStyle;
        std::string m_ImGuiIniPath = "imgui.ini";

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

        GLuint m_MsFboID = 0;            
        GLuint m_MsColorAttachmentID = 0;
        GLuint m_MsDepthAttachmentID = 0;

        int m_ViewportWidth = 0;
        int m_ViewportHeight = 0;
        int m_MsaaSamples = 4;
        int m_SelectedMsaaIndex = 0;
        float m_ViewportAspectRatio = 1.0f;
        std::vector<int> m_MsaaSampleOptions;
        std::vector<const char*> m_MsaaSampleLabels;

        static Application *s_Instance;
#if PLATFORM_EMSCRIPTEN
        static void emscriptenMainLoop(void *arg);
#endif
    };

} // namespace Base