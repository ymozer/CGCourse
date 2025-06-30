// clang-format off
// GLAD should be included first.
#if PLATFORM_DESKTOP
    #include <glad/gl.h>
#elif PLATFORM_ANDROID
    // On Android, we need both EGL and GLES headers
    #include <glad/egl.h>
    #include <glad/gles2.h>
    #define IMGUI_IMPL_OPENGL_ES3
#elif PLATFORM_EMSCRIPTEN
    #include <glad/gles2.h>
    #define IMGUI_IMPL_OPENGL_ES3
#endif

#include <SDL3/SDL.h>
#if PLATFORM_ANDROID
    #include <SDL3/SDL_filesystem.h>
#endif

#if PLATFORM_EMSCRIPTEN
    #include <emscripten.h>
#endif

#ifdef PLATFORM_WINDOWS
#define NOMINMAX
#include <windows.h>
#endif

#include <stdexcept>
#include <chrono>
#include <thread>
#include <utility>
#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_opengl3.h>
#include <spdlog/spdlog.h>

#include "Application.hpp"
#include "Shader.hpp"
#include "Input.hpp"
#include "Debug.hpp"
#include "Log.hpp"
// clang-format on

namespace Base
{
    Application *Base::Application::s_Instance = nullptr;

    Application::Application(std::string title, int width, int height, int numOfThreads)
        : m_Title(std::move(title)), m_Width(width), m_Height(height), m_EventBus(numOfThreads)
    {
        s_Instance = this;
    }

    Application::~Application()
    {
        LOG_INFO("Destroying Application");
        s_Instance = nullptr;
    }

    void Application::run()
    {
        init();
        LOG_INFO(">>>>>> Application::init() completed. Starting main loop... <<<<<<");

#if PLATFORM_EMSCRIPTEN
        emscripten_set_main_loop_arg(emscriptenMainLoop, this, 0, 1);
#else
        while (m_Running)
        {
            mainLoopIteration();
        }
        LOG_INFO(">>>>>> Main loop has exited. Cleaning up... <<<<<<");
        cleanup();
#endif
    }

    void Application::init()
    {
        LOG_INFO("Application created. Title: {} Size: {}x{}", m_Title, m_Width, m_Height);

        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            throw std::runtime_error("Failed to initialize SDL: " + std::string(SDL_GetError()));
        }

#if PLATFORM_DESKTOP
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#elif PLATFORM_EMSCRIPTEN || PLATFORM_IOS || PLATFORM_ANDROID
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengles2");
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_HIGH_PIXEL_DENSITY);
#if PLATFORM_DESKTOP
        window_flags = (SDL_WindowFlags)(window_flags | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);
#elif PLATFORM_IOS || PLATFORM_ANDROID
        window_flags = (SDL_WindowFlags)(window_flags | SDL_WINDOW_FULLSCREEN);
#elif PLATFORM_EMSCRIPTEN
        window_flags = (SDL_WindowFlags)(window_flags | SDL_WINDOW_MAXIMIZED);
#endif

        appContext.window = SDL_CreateWindow(m_Title.c_str(), m_Width, m_Height, window_flags);
        if (!appContext.window)
        {
            SDL_Quit();
            throw std::runtime_error("Failed to create SDL window: " + std::string(SDL_GetError()));
        }

        appContext.glcontext = SDL_GL_CreateContext(appContext.window);
        if (appContext.glcontext == nullptr)
        {
            SDL_DestroyWindow(appContext.window);
            SDL_Quit();
            throw std::runtime_error("Failed to create OpenGL context: " + std::string(SDL_GetError()));
        }
        SDL_GL_MakeCurrent(appContext.window, appContext.glcontext);
        SDL_GL_SetSwapInterval(m_VSync ? 1 : 0);

        SDL_GetWindowSizeInPixels(appContext.window, &m_Width, &m_Height);
        SDL_ShowWindow(appContext.window);

        m_AppQuitSubscription = m_EventBus.subscribe<ApplicationQuitEvent>([this](ApplicationQuitEvent &event)
                                                                           {
        LOG_INFO("ApplicationQuitEvent received, initiating shutdown.");
        this->m_Running = false; // m_Running should be std::atomic<bool> for thread safety
        event.handled = true; });

        m_WindowCloseSubscription = m_EventBus.subscribe<WindowCloseEvent>([this](WindowCloseEvent &event)
                                                                           {
        LOG_INFO("WindowCloseEvent received, initiating shutdown.");
        this->m_Running = false;
        event.handled = true; });

        if (!Input::Get().Initialize(m_EventBus, m_imGuiEnabled))
        {
            LOG_CRITICAL("Failed to initialize Input system!");
        }

#if PLATFORM_ANDROID
        EGLDisplay display = SDL_EGL_GetCurrentDisplay();
        if (display == EGL_NO_DISPLAY)
        {
            throw std::runtime_error("Failed to get EGL display from SDL");
        }
        if (!gladLoadEGL(display, (GLADloadfunc)SDL_GL_GetProcAddress))
        {
            throw std::runtime_error("Failed to initialize EGL with GLAD");
        }
        if (!gladLoadGLES2((GLADloadfunc)SDL_GL_GetProcAddress))
        {
            throw std::runtime_error("Failed to initialize GLES2 with GLAD");
        }
#elif PLATFORM_DESKTOP
        if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress))
        {
            throw std::runtime_error("Failed to initialize GLAD for Desktop GL");
        }
#elif PLATFORM_EMSCRIPTEN || PLATFORM_IOS
        if (!gladLoadGLES2((GLADloadfunc)SDL_GL_GetProcAddress))
        {
            throw std::runtime_error("Failed to initialize GLES2 with GLAD");
        }
#endif

        Base::Debug::enableGLDebugOutput();

        LOG_INFO("OpenGL Version: {}", reinterpret_cast<const char *>(glGetString(GL_VERSION)));
        m_PerfCounterFreq = SDL_GetPerformanceFrequency();
        m_LastFrameTimeCounter = SDL_GetPerformanceCounter();

        updateRenderingAndWorkAreas();

        createFramebuffer();
#if PLATFORM_DESKTOP
        glGenQueries(2, m_GpuTimeQueries);
#endif
        initImGui();
        setup();
    }

    void Application::handleEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);

            switch (event.type)
            {
            case SDL_EVENT_QUIT:
            {
                ApplicationQuitEvent quitEvent{};
                m_EventBus.dispatch(quitEvent);
                break;
            }
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            {
                WindowCloseEvent closeEvent(event.window.windowID);
                m_EventBus.dispatch(closeEvent);
                break;
            }

            case SDL_EVENT_WINDOW_RESIZED:
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            {
                SDL_GetWindowSizeInPixels(appContext.window, &m_Width, &m_Height);
                m_isMinimized = false;
                updateRenderingAndWorkAreas();
                LOG_INFO("Window resized to {}x{}", m_Width, m_Height);
                m_EventBus.dispatchAsync(WindowResizeEvent(event.window.windowID, m_Width, m_Height));

                break;
            }
            case SDL_EVENT_WINDOW_SAFE_AREA_CHANGED:
            {
                updateRenderingAndWorkAreas();
                break;
            }
            case SDL_EVENT_WINDOW_MINIMIZED:
            {
                m_isMinimized = true;
                m_EventBus.dispatchAsync(WindowMinimizeEvent(event.window.windowID));
                break;
            }
            case SDL_EVENT_WINDOW_RESTORED:
            {
                m_isMinimized = false;
                m_EventBus.dispatchAsync(WindowRestoreEvent(event.window.windowID));
                break;
            }
            case SDL_EVENT_KEY_DOWN:
            {
                if (!ImGui::GetIO().WantCaptureKeyboard) // Only dispatch if ImGui doesn't want it
                {
                    m_EventBus.dispatchAsync(KeyPressedEvent(
                        event.key.scancode, event.key.key,
                        event.key.mod, event.key.repeat != 0));
                }
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            {
                if (!ImGui::GetIO().WantCaptureMouse)
                {
                    m_EventBus.dispatchAsync(MouseButtonPressedEvent(
                        event.button.button, event.button.clicks,
                        event.button.x, event.button.y, event.button.windowID));
                }
                break;
            }
            }
        }
    }

    void Application::updateRenderingAndWorkAreas()
    {
        // This area should be the maximum usable space on the display.
        SDL_Rect usableBounds;
        SDL_DisplayID displayID = SDL_GetDisplayForWindow(appContext.window);
        if (!SDL_GetDisplayUsableBounds(displayID, &usableBounds))
        {
            LOG_WARN("Could not get display usable bounds: {}. Using full window.", SDL_GetError());
            SDL_GetWindowSizeInPixels(appContext.window, &usableBounds.w, &usableBounds.h);
            usableBounds.x = 0;
            usableBounds.y = 0;
        }

        int winX, winY;
        SDL_GetWindowPosition(appContext.window, &winX, &winY);
        int winW, winH;
        SDL_GetWindowSizeInPixels(appContext.window, &winW, &winH);

        int overlap_x1 = std::max(winX, usableBounds.x);
        int overlap_y1 = std::max(winY, usableBounds.y);
        int overlap_x2 = std::min(winX + winW, usableBounds.x + usableBounds.w);
        int overlap_y2 = std::min(winY + winH, usableBounds.y + usableBounds.h);

        m_RenderArea.x = overlap_x1 - winX;
        m_RenderArea.y = overlap_y1 - winY;
        m_RenderArea.w = std::max(0, overlap_x2 - overlap_x1);
        m_RenderArea.h = std::max(0, overlap_y2 - overlap_y1);
        LOG_INFO("Render Area updated to: x={}, y={}, w={}, h={}", m_RenderArea.x, m_RenderArea.y, m_RenderArea.w, m_RenderArea.h);

#if PLATFORM_IOS || PLATFORM_ANDROID
        // On mobile, the UI is constrained to the safe area.
        if (!SDL_GetWindowSafeArea(appContext.window, &m_WorkArea))
        {
            LOG_WARN("Could not get window safe area: {}. Using full window.", SDL_GetError());
            m_WorkArea = m_RenderArea;
        }
#else
        m_WorkArea.x = 0;
        m_WorkArea.y = 0;
        m_WorkArea.w = m_Width;
        m_WorkArea.h = m_Height;
#endif
        LOG_INFO("UI Work Area updated to: x={}, y={}, w={}, h={}", m_WorkArea.x, m_WorkArea.y, m_WorkArea.w, m_WorkArea.h);
    }

    void Application::mainLoopIteration()
    {
        uint64_t frameStartTimeCounter = SDL_GetPerformanceCounter();
        uint64_t cpuWorkStartTimeCounter = 0;

        handleEvents();
        if (!m_Running)
            return;

        if (m_isMinimized)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            return;
        }

        cpuWorkStartTimeCounter = SDL_GetPerformanceCounter();

#if PLATFORM_DESKTOP
        if (m_FrameCount > 0)
        {
            GLint query_available = 0;
            while (!query_available)
            {
                glGetQueryObjectiv(m_GpuTimeQueries[1], GL_QUERY_RESULT_AVAILABLE, &query_available);
            }
            GLuint64 gpu_time_start, gpu_time_end;
            glGetQueryObjectui64v(m_GpuTimeQueries[0], GL_QUERY_RESULT, &gpu_time_start);
            glGetQueryObjectui64v(m_GpuTimeQueries[1], GL_QUERY_RESULT, &gpu_time_end);
            m_GpuTime_ms = (gpu_time_end - gpu_time_start) / 1000000.0f;
        }
#else
        m_GpuTime_ms = 0.0f;
#endif

        float deltaTime = (float)((double)(frameStartTimeCounter - m_LastFrameTimeCounter) / m_PerfCounterFreq);
        m_LastFrameTimeCounter = frameStartTimeCounter;
        update(deltaTime);

#if PLATFORM_DESKTOP
        glQueryCounter(m_GpuTimeQueries[0], GL_TIMESTAMP);
#endif

        beginImGuiFrame();
        renderUI();

        // Render scene to FBO
        glBindFramebuffer(GL_FRAMEBUFFER, m_FboID);
        glViewport(0, 0, m_ViewportWidth, m_ViewportHeight);
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render();

        // Prepare to render to main window
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_Width, m_Height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        endImGuiFrame();

#if PLATFORM_DESKTOP
        glQueryCounter(m_GpuTimeQueries[1], GL_TIMESTAMP);
#endif

        m_CpuTime_ms = (float)(((double)(SDL_GetPerformanceCounter() - cpuWorkStartTimeCounter) * 1000.0) / m_PerfCounterFreq);
        SDL_GL_SwapWindow(appContext.window);

        if (!m_VSync)
        {
            double targetFrameTime = 1.0 / m_FpsLimit;
            double frameDuration = (double)(SDL_GetPerformanceCounter() - frameStartTimeCounter) / m_PerfCounterFreq;
            if (frameDuration < targetFrameTime)
            {
                SDL_Delay((Uint32)((targetFrameTime - frameDuration) * 1000.0));
            }
        }

        m_FrameCount++;
    }

#if PLATFORM_EMSCRIPTEN
    void Application::emscriptenMainLoop(void *arg) { static_cast<Application *>(arg)->mainLoopIteration(); }
#endif

    void Application::cleanup()
    {
        shutdown();
        m_EventBus.unsubscribe(m_KeySub);
        m_EventBus.unsubscribe(m_MouseSub);

        m_EventBus.unsubscribe(m_AppQuitSubscription);
        m_EventBus.unsubscribe(m_WindowCloseSubscription);

#if PLATFORM_DESKTOP
        glDeleteQueries(2, m_GpuTimeQueries);
#endif
        cleanupFramebuffer();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();

        SDL_GL_DestroyContext(appContext.glcontext);
        SDL_DestroyWindow(appContext.window);
        SDL_Quit();
    }

    void Application::initImGui()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#if PLATFORM_EMSCRIPTEN
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.IniFilename = nullptr;
#else
        // This returns a platform-specific, writable directory for app preferences.
        // e.g., on Windows: C:/Users/YourUser/AppData/Roaming/YourOrg/YourApp
        // e.g., on Linux:   ~/.local/share/YourOrg/YourApp
        // e.g., on macOS:   ~/Library/Application Support/YourOrg/YourApp
        char *pref_path = SDL_GetPrefPath("com.libsdl.app", "CGCourse");
        if (pref_path)
        {
            m_ImGuiIniPath = std::string(pref_path) + "imgui.ini";
            SDL_free(pref_path);

            io.IniFilename = m_ImGuiIniPath.c_str();
            LOG_INFO("ImGui .ini path set to: {}", io.IniFilename);
        }
        else
        {
            io.IniFilename = "imgui.ini";
            LOG_WARN("Could not get preference path from SDL. Defaulting to local 'imgui.ini'");
        }
#endif
        ImGui::StyleColorsDark();
        m_BaseStyle = ImGui::GetStyle();

        float xscale = SDL_GetWindowDisplayScale(appContext.window);
        m_StyleScale = xscale;
        m_FontScale = xscale;

        io.Fonts->AddFontDefault();
        ImGuiStyle &style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }
        style.ScaleAllSizes(m_StyleScale);

        ImGui_ImplSDL3_InitForOpenGL(appContext.window, appContext.glcontext);
#if defined(IMGUI_IMPL_OPENGL_ES3)
        const char *glsl_version = "#version 300 es";
#elif defined(IMGUI_IMPL_OPENGL_ES2)
        const char *glsl_version = "#version 100";
#else // non-es
        const char *glsl_version = "#version 410";
#endif
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    void Application::beginImGuiFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ImGuiViewport *viewport = ImGui::GetMainViewport();
        viewport->WorkPos = ImVec2((float)m_WorkArea.x, (float)m_WorkArea.y);
        viewport->WorkSize = ImVec2((float)m_WorkArea.w, (float)m_WorkArea.h);
    }

    void Application::endImGuiFrame()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            SDL_Window *backup_current_window = SDL_GL_GetCurrentWindow();
            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        }
    }

    void Application::renderUI()
    {
        static bool dockspaceOpen = true;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
        ImGuiViewport *main_viewport = ImGui::GetMainViewport();

        // Set the main dockspace to fill the work area defined in the viewport
        ImGui::SetNextWindowPos(ImVec2((float)m_RenderArea.x, (float)m_RenderArea.y));
        ImGui::SetNextWindowSize(ImVec2((float)m_RenderArea.w, (float)m_RenderArea.h));
        ImGui::SetNextWindowViewport(main_viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 10.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
        {
            ImGui::PopStyleVar();
            ImGui::PopStyleVar(2);

            ImGuiIO &io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
            }

            static bool show_borders = false;
            if (show_borders)
            {
                ImDrawList *draw_list = ImGui::GetForegroundDrawList();
                // Green for safe UI Work Area
                ImVec2 work_min = ImVec2((float)m_WorkArea.x, (float)m_WorkArea.y);
                ImVec2 work_max = ImVec2(work_min.x + m_WorkArea.w, work_min.y + m_WorkArea.h);
                draw_list->AddRect(work_min, work_max, IM_COL32(0, 255, 0, 255), 0.0f, 0, 5.0f);
                // Blue for full Render Area
                ImVec2 render_min = ImVec2((float)m_RenderArea.x, (float)m_RenderArea.y);
                ImVec2 render_max = ImVec2(render_min.x + m_RenderArea.w, render_min.y + m_RenderArea.h);
                draw_list->AddRect(render_min, render_max, IM_COL32(0, 0, 255, 255), 0.0f, 0, 2.0f);
            }

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
            ImGui::Begin("Viewport");
            {
                ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
                if (m_ViewportWidth != (int)viewportPanelSize.x || m_ViewportHeight != (int)viewportPanelSize.y)
                {
                    resizeFramebuffer((int)viewportPanelSize.x, (int)viewportPanelSize.y);
                }
                ImGui::Image((ImTextureID)(intptr_t)m_ColorAttachmentID, viewportPanelSize, ImVec2(0, 1), ImVec2(1, 0));
            }
            ImGui::End();
            ImGui::PopStyleVar();

            ImGui::Begin("Debug Info");
            {
                ImGui::Text("Window Size: %d x %d", m_Width, m_Height);
                ImGui::Text("Work Area: x:%d y:%d w:%d h:%d", m_WorkArea.x, m_WorkArea.y, m_WorkArea.w, m_WorkArea.h);
                ImGui::Text("Viewport Size: %d x %d", m_ViewportWidth, m_ViewportHeight);
                ImGui::Separator();
                if (ImGui::Checkbox("V-Sync", &m_VSync))
                {
                    SDL_GL_SetSwapInterval(m_VSync ? 1 : 0);
                }
                ImGui::BeginDisabled(m_VSync);
                ImGui::SliderInt("FPS Limit", &m_FpsLimit, 30, 300);
                ImGui::EndDisabled();
                ImGui::Separator();
                ImGui::Text("Performance");
                ImGui::Text("FPS: %.1f", io.Framerate);
                ImGui::Text("CPU Time: %.3f ms", m_CpuTime_ms);
                ImGui::Text("GPU Time: %.3f ms", m_GpuTime_ms);
                ImGui::Separator();

                ImGui::Text("UI Scale");
                if (ImGui::SliderFloat("##StyleScale", &m_StyleScale, 1.0f, 5.0f, "%.2f"))
                {
                    ImGui::GetStyle() = m_BaseStyle;
                    ImGui::GetStyle().ScaleAllSizes(m_StyleScale);
                }

                ImGui::Text("Font Scale");
                static float lastFontScale = m_FontScale;
                if (ImGui::SliderFloat("##FontScale", &m_FontScale, 1.0f, 5.0f, "%.2f"))
                {
                    float relativeScale = m_FontScale / lastFontScale;
                    io.FontGlobalScale *= relativeScale;
                    lastFontScale = m_FontScale;
                }

                ImGui::SameLine();
                if (ImGui::Button("Reset"))
                {
                    // Get the monitor's native scale
                    float xscale = SDL_GetWindowDisplayScale(appContext.window);

                    // Reset Style
                    m_StyleScale = xscale;
                    ImGui::GetStyle() = m_BaseStyle;
                    ImGui::GetStyle().ScaleAllSizes(m_StyleScale);

                    // Reset Font
                    m_FontScale = xscale;
                    lastFontScale = xscale;
                    io.FontGlobalScale = 1.0f;
                }
                ImGui::Separator();
                ImGui::Checkbox("Show Work Area Border", &show_borders);
                ImGui::Separator();
            }
            ImGui::End();
            renderChapterUI();
        }
        ImGui::End();
    }

    void Application::createFramebuffer()
    {
        m_ViewportWidth = m_RenderArea.w > 0 ? m_RenderArea.w : 1;
        m_ViewportHeight = m_RenderArea.h > 0 ? m_RenderArea.h : 1;

        glGenFramebuffers(1, &m_FboID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FboID);

        glGenTextures(1, &m_ColorAttachmentID);
        glBindTexture(GL_TEXTURE_2D, m_ColorAttachmentID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_ViewportWidth, m_ViewportHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachmentID, 0);

        glGenRenderbuffers(1, &m_DepthAttachmentID);
        glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachmentID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_ViewportWidth, m_ViewportHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthAttachmentID);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            LOG_ERROR("Framebuffer is not complete!");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Application::resizeFramebuffer(int width, int height)
    {
        if (width <= 0 || height <= 0 || (m_ViewportWidth == width && m_ViewportHeight == height))
            return;

        m_ViewportWidth = width;
        m_ViewportHeight = height;

        glBindTexture(GL_TEXTURE_2D, m_ColorAttachmentID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachmentID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

        LOG_INFO("Framebuffer resized to {}x{}", width, height);
    }

    void Application::cleanupFramebuffer()
    {
        glDeleteFramebuffers(1, &m_FboID);
        glDeleteTextures(1, &m_ColorAttachmentID);
        glDeleteRenderbuffers(1, &m_DepthAttachmentID);
    }
} // namespace Base